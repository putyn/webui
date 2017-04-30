#include "webui.h"
extern "C" {
	#include "user_interface.h"
}

//web server and dns server
AsyncWebServer server(80);
DNSServer dns_server;

wifi_settings_t wifi;
settings_t settings;


void wifi_setup() {
	
  //nice hostname
  WiFi.hostname(settings.hostname);
  WiFi.mode(WIFI_STA);
  
  //check to see if we have config file
  Serial.println();
  Serial.println(F("[WiFi] checking to see if we have new config..."));
  if (SPIFFS.exists("/wifi.dat")) {
    //read wifi settings from file
    read_file((char *)"/wifi.dat", (byte *)&wifi, sizeof(struct wifi_settings_t));
    WiFi.begin(wifi.ssid, wifi.pass);
    //remove config
    SPIFFS.remove("/wifi.dat");
  } else {
    //try to connect to previous network if any else start in AP mode
    Serial.println(F("[WiFi] connecting to existing network..."));
    WiFi.begin();
  }

  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    Serial.printf("[WiFi] successfully connected to ssid: %s\n", WiFi.SSID().c_str());
    Serial.printf("[WiFi] IP addess: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println(F("[WiFi] configuring access point..."));
    WiFi.mode(WIFI_AP_STA);
    WiFi.disconnect();
    if (WiFi.softAP(settings.hostname)) {
      
      Serial.printf("[WiFi] successfully created access point: %s\n", settings.hostname);
      Serial.printf("[WiFi] IP addess: %s\n", WiFi.softAPIP().toString().c_str());
      
      //start DNS server for easier configuration
      Serial.println(F("[WiFi] starting DNS server"));
      dns_server.start(53, "*", WiFi.softAPIP());
      settings.soft_ap = true;
      
    } else {
      Serial.println(F("[WiFi] something failed, halting"));
      while (1);
    }
  }
  
  WiFi.scanNetworks(1);
  Serial.println(F("[WiFi] starting network scanning async"));
}

void web_setup() {
	
	server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request ) {
		settings.reboot = true;
		request->redirect("/");
	});
	
	server.on("/wifi", HTTP_GET,  handle_wifi);
	server.on("/time", HTTP_GET,  handle_time);
	server.on("/overview", HTTP_GET,  handle_overview);
	server.on("/wifi", HTTP_POST,  handle_wifi_save);
	server.on("/time", HTTP_POST,  handle_time_save);
	server.serveStatic("/", SPIFFS, "/").setCacheControl("max-age:600");

	Serial.println(F("[HTTP] Starting HTTP server"));
	server.begin();
}

/**
 *	dummy wrapper for dns requests
 */
void webui_dns_requests()  {
	dns_server.processNextRequest();
}

void fs_setup() {
	
	Serial.println();
	if (SPIFFS.begin()) {
		Serial.println(F("[FS] file system opened, generating file list"));
		
		//file list
		Dir dir = SPIFFS.openDir("/");
		while (dir.next()) {
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
			Serial.printf("[FS] file: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
		}
		
		//load settings
		if (SPIFFS.exists("/settings.dat")) {
			read_file((char *)"/settings.dat", (byte *)&settings, sizeof(struct settings_t));
			Serial.printf("[FS] time server: %s\n", settings.time_server);
			Serial.printf("[FS] time zone: %d\n", settings.time_zone);
			Serial.printf("[FS] time DST: %d\n", settings.time_dst);
		}
		
		//settings that are not saved
		settings.reboot = false;
		settings.soft_ap = false;
		settings.online = false;
		settings.update_time = false;
		settings.update_display = false;
  
		
	} else {
		Serial.println(F("[FS] file system couldn't be opened, halting"));
		while(1);
	}
} 
/**
 * some system information
 */
void handle_overview(AsyncWebServerRequest *request) {

  String json_resp;
  char chipid[6] = {0};
  sprintf(chipid, "%06X", ESP.getChipId());
   
  json_resp = "{\"ChipID\": \"" + String(chipid) + "\",";
  json_resp += "\"CPU frequency\": \"" + String(ESP.getCpuFreqMHz()) + " MHz\",";
  json_resp += "\"Last reset reason\": \"" + ESP.getResetReason() + "\",";
  json_resp += "\"Internet\": \"" + String(settings.online ? "online" : "offline") + "\",";
  json_resp += "\"Uptime\": \"" + String(millis()/1000) + "s\",";
  json_resp += "\"Free heap\": \"" + formatBytes(ESP.getFreeHeap()) + "\",";
  json_resp += "\"Storage size\": \"" + formatBytes(ESP.getFlashChipSize()) + "\",";
  json_resp += "\"Firmware size\": \"" + formatBytes(ESP.getSketchSize()) + "\",";
  json_resp += "\"Free firmware space\": \"" + formatBytes(ESP.getFreeSketchSpace()) + "\"}";
  
  request->send(200, "text/json", json_resp);
}
/**
 * handle wifi setting save
 */
void handle_wifi_save(AsyncWebServerRequest *request) {
  //json response
  String json_resp;
  
  if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
    
    //get ssid and pass from POST request
    strncpy(wifi.ssid, request->getParam("ssid", true)->value().c_str(), sizeof (wifi.ssid));
    strncpy(wifi.pass, request->getParam("pass", true)->value().c_str(), sizeof (wifi.pass));

    if (save_file((char *)"/wifi.dat", (byte *)&wifi, sizeof(struct wifi_settings_t))) {
      json_resp = F("{\"error\":false, \"message\": \"Settings saved, reboot to load new network config\"}");
    } else {
      json_resp = F("{\"error\":true, \"message\": \"Settings could not be saved\"}");
    }
  } else {
    json_resp = F("{\"error\":true, \"message\": \"Bad request\"}");
  }
  request->send(200, "text/json", json_resp);
}
/**
 * Handle [GET] /wifi
 * returns wifi status
 * available network list
 */
void handle_wifi(AsyncWebServerRequest *request) {

  uint8_t found_networks = 0;
  uint8_t idx = 0;
  String json_resp;

  if ((WiFi.getMode() & WIFI_AP_STA) == WIFI_AP_STA) {
    json_resp = "{\"status\":{\"Mode\":\"Access Point\",\"IP\":\"" + WiFi.softAPIP().toString() + "\",\"Hostname\":\"" + WiFi.hostname() + "\"},\"networks\":[";
  } else {
    json_resp = "{\"status\":{\"Mode\":\"Client\",\"SSID\":\"" + WiFi.SSID() + "\",\"IP\":\"" + WiFi.localIP().toString() + "\",\"Hostname\":\"" + WiFi.hostname() + "\"},\"networks\":[";
  }

  found_networks = WiFi.scanComplete();
  if ( found_networks > 0) {
    for (idx = 0; idx < found_networks; ++idx) {
      if (idx)
        json_resp += ",";
      json_resp += "{\"ssid\":\"" + WiFi.SSID(idx) + "\",\"auth\":" + (WiFi.encryptionType(idx) == ENC_TYPE_NONE ? 0 : 1) + ",\"quality\":" + rssi2quality(WiFi.RSSI(idx)) + "}";
    }
    WiFi.scanDelete();
    WiFi.scanNetworks(1);
  }
  json_resp += "]}";
  request->send(200, "text/json", json_resp);
}
/**
 * placeholder template for time, untill its finished
 */
void handle_time(AsyncWebServerRequest *request) {
  
  String json_resp = "";

  json_resp = "{\"time_server\":\""+ String(settings.time_server) +"\",";
  if(settings.time_zone > 0) {
    json_resp += "\"time_zone\":\"+" + String(settings.time_zone) + "\",";  
  } else {
    json_resp += "\"time_zone\":" + String(settings.time_zone) + ",";
  }
  json_resp +=  "\"time_dst\": "+ String(settings.time_dst) +"}";
  request->send(200, "text/json", json_resp);
}

void handle_time_save(AsyncWebServerRequest *request) {
  //json response
  String json_resp;
  
  if (request->hasParam("time_server", true) && request->hasParam("time_zone", true) && request->hasParam("time_dst", true)) {
    
    //get time_server, time_zone, time_dst from POST request
    strncpy(settings.time_server, request->getParam("time_server", true)->value().c_str(), sizeof (settings.time_server));
    settings.time_zone = request->getParam("time_zone", true)->value().toInt();
    settings.time_dst  = request->getParam("time_dst", true)->value().toInt();

    if (save_file((char *)"/settings.dat", (byte *)&settings, sizeof(struct settings_t))) {
      json_resp = F("{\"error\":false, \"message\": \"\"}");
    } else {
      json_resp = F("{\"error\":true, \"message\": \"Settings could not be saved\"}");
    }
  } else {
    json_resp = F("{\"error\":true, \"message\": \"Bad request\"}");
  }
  request->send(200, "text/json", json_resp);
}
/*
 * helper functions
 *
 * return size of file human readable
 */
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}
/*
 * rssi to quality for sorting networks
 * based on http://www.speedguide.net/faq/how-does-rssi-dbm-relate-to-signal-quality-percent-439
 */
uint8_t rssi2quality(int16_t rssi) {
  uint8_t quality;

  if ( rssi <= -100) {
    quality = 0;
  } else if ( rssi >= -50) {
    quality = 100;
  } else {
    quality = 2 * ( rssi + 100);
  }
  return quality;
}
/*
 * helper function for saving data structures to file
 * havely based on  https://github.com/letscontrolit/ESPEasy/blob/mega/src/Misc.ino#L767
 */
boolean save_file(char* fname, byte* memAddress, int datasize){

  fs::File f = SPIFFS.open(fname, "w+");
  if (f) {
    byte *pointerToByteToSave = memAddress;
    for (int x = 0; x < datasize ; x++) {
      f.write(*pointerToByteToSave);
      pointerToByteToSave++;
    }
    f.close();
  }
  return true;
}
/*
 * helper function for reading data structures from file
 * havely based on  https://github.com/letscontrolit/ESPEasy/blob/mega/src/Misc.ino#L801
 */
void read_file(char* fname, byte* memAddress, int datasize) {
  fs::File f = SPIFFS.open(fname, "r+");
  if (f) {
    byte *pointerToByteToRead = memAddress;
    for (int x = 0; x < datasize; x++) {
      *pointerToByteToRead = f.read();
      pointerToByteToRead++;// next byte
    }
    f.close();
  }
}

/** 
 * ntp_time
 */
WiFiUDP ntp;
#define NTP_LOCAL_PORT 8266
#define NTP_PACKET_SIZE 48
#define NTP_TIMESTAMP_DELTA 2208988800UL //Diff btw a UNIX timestamp (Starting Jan, 1st 1970) and a NTP timestamp (Starting Jan, 1st 1900)
#define NTP_UPDATE 15 * 60 * 1000 //15 min 
 
/**
 * get time via NTP
 * returns time in the custom structure
 * based on https://www.arduino.cc/en/Tutorial/UdpNTPClient
 *			http://playground.arduino.cc/Code/NTPclient
 */
uint32_t ntp_get_time() {
  
	//start udp client
	ntp.begin(NTP_LOCAL_PORT);
  
	//debug info
	Serial.printf("[NTP] UDP client on port: %d\n", ntp.localPort());
  
	//ntp ip
	IPAddress ntp_server_ip;
  
	//ntp buffer
	uint8_t ntp_packet[NTP_PACKET_SIZE];
	memset(ntp_packet, 0, NTP_PACKET_SIZE);
  
	//time
	uint32_t temp_seconds = 0;
  
	//get ip from the pool
	if(!WiFi.hostByName(settings.time_server, ntp_server_ip)) {
		Serial.printf("[NTP] can't get ip for host %s\n", settings.time_server);
		return 0;  
	}
  
	//debug info
	Serial.printf("[NTP] server IP from pool: %s\n",ntp_server_ip.toString().c_str());
  
	//mk ntp packet
	ntp_packet[0] = 0xE3;
	ntp_packet[1] = 0x00;
	ntp_packet[2] = 0x06;
	ntp_packet[3] = 0xEC;
  
	ntp.beginPacket(ntp_server_ip, 123);
	ntp.write(ntp_packet, NTP_PACKET_SIZE);
	ntp.endPacket();

	//wait until we get a packet with timeout
	uint32_t ntp_time_out = millis();
	boolean ntp_got_packet = false;
	while (!(millis() > ntp_time_out + 5000)) {
		if(ntp.parsePacket() == NTP_PACKET_SIZE) {
			Serial.printf("[NTP] got NTP packet in %d\n",millis() - ntp_time_out);
			ntp_got_packet = true;
			break;
		}
	}

  if(!ntp_got_packet) {
    Serial.println(F("[NTP] didn't get a message from the server, closing connection"));
    settings.next_ntp_update = millis() + 60 * 1000; //force update in 1 min
    ntp.stop();
    return 0;
  }
  
  //read the packet
  memset(ntp_packet,0, NTP_PACKET_SIZE);
  ntp.read(ntp_packet, NTP_PACKET_SIZE);
  
  /*
  uint8_t idx_pkt;
  for(idx_pkt = 0; idx_pkt < NTP_PACKET_SIZE; idx_pkt++)
	Serial.printf("Pkt %d=0x%x\n", idx_pkt, ntp_packet[idx_pkt]);
  */

  //NTP time, seconds since Jan, 1st 1900
  temp_seconds = (word(ntp_packet[40], ntp_packet[41])) << 16 | word(ntp_packet[42], ntp_packet[43]); 
  
  //Unix time, seconds since Jan, 1st 1970 + time zone +/- dst
  temp_seconds = temp_seconds - NTP_TIMESTAMP_DELTA + settings.time_zone + settings.time_dst;
  
  /*
  // hour, minute and second:
  tmp_time.hours = (epoch  % 86400L) / 3600;
  tmp_time.minutes = (epoch  % 3600) / 60;
  tmp_time.seconds = epoch  % 60;
  */
  
  //set next ntp update
  settings.next_ntp_update = millis() + NTP_UPDATE;
  ntp.stop();
  
  return temp_seconds;
}
