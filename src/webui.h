#pragma once

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

//structures
struct wifi_settings_t {
  char ssid[32];
  char pass[32];
}; 

struct settings_t {
  char time_server[32];
  char hostname[32];
  int16_t time_zone;
  int16_t time_dst;
  boolean reboot;
  boolean online;
  boolean soft_ap;
};

/*
 * Web Server
 * WiFi
 * File System
 * DNS
 */
void wifi_setup();
void web_setup();
void fs_setup();
void webui_dns_requests();

/*
 * handles for webserver
 */
void handle_overview(AsyncWebServerRequest *request);
void handle_wifi(AsyncWebServerRequest *request);
void handle_wifi_save(AsyncWebServerRequest *request);
void handle_time(AsyncWebServerRequest *request);
void handle_time_save(AsyncWebServerRequest *request);

/*
 * misc utility functions
 */
 
String formatBytes(size_t bytes);
int rssi2quality(int rssi);
boolean save_file(char* fname, byte* memAddress, int datasize);
void read_file(char* fname, byte* memAddress, int datasize);
