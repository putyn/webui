//https://github.com/putyn/webui
#include <webui.h>
#include <Ticker.h>

/*
 * settings from webui.h
 * device settings from webui.h
 */
extern settings_t settings;
extern device_t device;

/*
 * time
 */
ctime_t local_time;

/*
 * needed by the webui
 */
void hw_set_brightness(uint8_t brightness) {
  
}
/*
 * ticker instance to count uptime
 */
Ticker tick;

void tock() {
  local_time.millis++;

  if (local_time.millis > 999){
    local_time.millis = 0;
    device.uptime++;
  }
}

void setup() {
  //for debug reasonts
  Serial.begin(74880);

  //setup file system
  fs_setup();
  //set a nice hostname
  sprintf(device.hostname,"webui_dev_%06X", ESP.getChipId());

  //setup wifi
  wifi_setup(false);

  //setup web services
  web_setup();

  //Ticker
  tick.attach_ms(1,tock);
}

void loop() {
  if(device.reboot){
    Serial.println("[SYS] got restart request from the wire!");
    device.reboot = false;
    ESP.restart();
  }
  if(device.soft_ap){
    webui_dns_requests();
  }
  if(device.test_wifi){
    Serial.println("[WIFI] got request to test WIFI network");
    device.test_wifi = false;
    wifi_setup(true);
  }
}
