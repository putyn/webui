#include <webui.h>
//since this was defined in webui we make it external here
extern settings_t settings;

void setup() {
  //for debug reasonts
  Serial.begin(74880);

  //setup file system
  fs_setup();
  //set a nice hostname
  sprintf(settings.hostname,"big_seven_%06X", ESP.getChipId());

  //setup wifi
  wifi_setup();

  //setup web services
  web_setup();
}

void loop() {
  if(settings.reboot){
    Serial.println("[WiFi] got restart request from the wire!");
    settings.reboot = false;
    ESP.restart();
  }
  if(settings.soft_ap){
    webui_dns_requests();
  }
}
