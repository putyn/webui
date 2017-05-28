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

struct settings_t {
  char time_server[32];
  char hostname[32];
  uint8_t brightness;
  int16_t time_zone;
  int16_t time_dst;
  uint32_t next_ntp_update;
  uint32_t uptime;
  boolean update_display;
  boolean update_time;
  boolean reboot;
  boolean online;
  boolean soft_ap;
};

struct ctime_t {
	uint32_t millis;
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
};

/*
 * Web Server
 * WiFi
 * File System
 * DNS
 * NTP requests
 */
void wifi_setup();
void web_setup();
void fs_setup();
void webui_dns_requests();
int8_t ntp_get_time(ctime_t *temp_time);

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
