
#include <ArduinoOTA.h>

led_anim_t ota_anim_enabled[] = { {  10,  20 }, { 100,   20 }, {  10, 20 }, { 0,  50 }, { 10, 20 }, { 100, 20 }, { 10, 20 }, { 0, 750 }, LED_ANIM_LOOP };
led_anim_t ota_anim_active[] = { {  10,  20 }, { 100,   20 }, {  10, 20 }, { 0, 20 }, LED_ANIM_LOOP };

bool ota_started = false;

void ota_setup()
{
  if(ota_started)
  {
    return;
  }
  
  Serial.printf("[OTA] Starting...\n");
  ArduinoOTA.setHostname("LegoRemote");

  ArduinoOTA
    .onStart([]() {
      Serial.printf("[OTA] Start download...\n");
      ota_active = true;
      led_anim_blue(ota_anim_active);
      led_anim_red(ota_anim_active);
    })
    .onEnd([]() {
      Serial.printf("[OTA] Finished download...\n");
      ota_active = false;
      led_anim_blue(led_anim_none);
      led_anim_red(led_anim_none);
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      analogWrite(LED_STATUS, progress * 255 / total, 255);
    })
    .onError([](ota_error_t error) {
      led_anim_blue(led_anim_emergency);
      led_anim_red(led_anim_emergency);
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    
  ota_started = true;
  ArduinoOTA.begin();
  
  led_anim_blue(ota_anim_enabled);
}

bool ota_loop()
{
  if(ota_started)
  {
    ArduinoOTA.handle();
  }

  return ota_active;
}

