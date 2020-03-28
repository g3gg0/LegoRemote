
/* 
 *   To be built with partition scheme "Minimal SPIFFS"
 */

#include <WiFi.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <SPIFFS.h>
#include <BLEDevice.h>

#include "LED.h"

bool ota_active = false;


#define LED_ACTIVITY 13
#define LED_STATUS   15


void setup()
{
  Serial.begin(115200);
  Serial.printf("\n\n\n");

  Serial.printf("[i] SDK:          '%s'\n", ESP.getSdkVersion());
  Serial.printf("[i] CPU Speed:    %d MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("[i] Chip Id:      %06X\n", ESP.getEfuseMac());
  Serial.printf("[i] Flash Mode:   %08X\n", ESP.getFlashChipMode());
  Serial.printf("[i] Flash Size:   %08X\n", ESP.getFlashChipSize());
  Serial.printf("[i] Flash Speed:  %d MHz\n", ESP.getFlashChipSpeed() / 1000000);
  Serial.printf("[i] Heap          %d/%d\n", ESP.getFreeHeap(), ESP.getHeapSize());
  Serial.printf("[i] SPIRam        %d/%d\n", ESP.getFreePsram(), ESP.getPsramSize());
  Serial.printf("\n");
  Serial.printf("[i] Starting\n");

  Serial.printf("[i]   Setup LEDs\n");
  led_setup();
  Serial.printf("[i]   Setup LEGO outputs\n");
  lego_init();
  Serial.printf("[i]   Setup SPIFFS\n");
  if(!SPIFFS.begin(true))
  {
    Serial.println("[E]   SPIFFS Mount Failed");
  }
  config_setup();

  Serial.printf("[i]   Setup BLE\n");
  ble_setup();
  
  Serial.println("Setup done");
}

void loop()
{
  bool hasWork = false;

  hasWork |= lego_loop();
  hasWork |= config_loop();
  hasWork |= ble_loop();
  hasWork |= wifi_loop();
  hasWork |= led_loop();
  hasWork |= ota_loop();
  
  if(!hasWork)
  {
    delay(20);
  }
  else
  {
    delay(5);
  }
}

