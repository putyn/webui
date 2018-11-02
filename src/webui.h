#pragma once

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFIUDP.h>
#include <FS.h>

//structures
struct wifi_settings_t {
  char ssid[32];
  char pass[32];
}; 

struct ctime_t {
	uint32_t millis;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
};

/*
 * settings that should be saved
 */
struct settings_t {
	//time server, to add length check in ui
  char time_server[32];	
  //saved brightness level, between 0(lowest) -- 10(highest) 
  uint8_t brightness;
  //how often should ACP run min is 5 mins	
  uint8_t acp_time;
  //suppress ACP during night mode 	
  uint8_t suppress_acp;	
  //timezone + dst
  int16_t time_zone;
  int16_t time_dst;
  //begining & ending for night mode (during night mode lowest brightness is used, ACP is controlled via ui);
  ctime_t nightmode_start;
  ctime_t nightmode_stop;
};

/*
 * device settings
 */
struct device_t {
	char hostname[32];
	uint32_t next_ntp_update;
  uint32_t acp_start_time;
  uint32_t uptime;
  boolean should_acp;
  boolean nightmode;
  boolean acp_is_running;
  boolean update_display;
  boolean update_time;
  boolean reboot;
  boolean test_wifi;
  boolean online;
  boolean soft_ap;
};

/*
 * Web Server
 * WiFi
 * File System
 * DNS
 * NTP requests
 */
void wifi_setup(boolean test_wifi);
void web_setup();
void fs_setup();
void webui_dns_requests();
int8_t ntp_get_time(ctime_t *temp_time);
uint8_t is_night();

/*
 * extern functions
 */
extern void hw_set_brightness(uint8_t brightness);
/*
 * handles for webserver
 */
void handle_overview(AsyncWebServerRequest *request);
void handle_wifi(AsyncWebServerRequest *request);
void handle_wifi_save(AsyncWebServerRequest *request);
void handle_time(AsyncWebServerRequest *request);
void handle_time_save(AsyncWebServerRequest *request);
void handle_hw(AsyncWebServerRequest *request);
void handle_hw_save(AsyncWebServerRequest *request);
/*
 * misc utility functions
 */
 
String formatBytes(size_t bytes);
String mkuptime(uint32_t uptime	);
uint8_t rssi2quality(int16_t rssi);
boolean save_file(char* fname, byte* memAddress, int datasize);
void read_file(char* fname, byte* memAddress, int datasize);
