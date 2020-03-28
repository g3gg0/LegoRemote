
#include "LED.h"
#include "Config.h"

#define STEAM_CONTROLLER_BUTTON_A                  0x800000
#define STEAM_CONTROLLER_BUTTON_X                  0x400000
#define STEAM_CONTROLLER_BUTTON_B                  0x200000
#define STEAM_CONTROLLER_BUTTON_Y                  0x100000
#define STEAM_CONTROLLER_BUTTON_LEFT_UPPER_PADDLE  0x080000
#define STEAM_CONTROLLER_BUTTON_RIGHT_UPPER_PADDLE 0x040000
#define STEAM_CONTROLLER_BUTTON_LEFT_PADDLE        0x020000
#define STEAM_CONTROLLER_BUTTON_RIGHT_PADDLE       0x010000
#define STEAM_CONTROLLER_BUTTON_LEFT_INNER_PADDLE  0x008000
#define STEAM_CONTROLLER_BUTTON_NAV_RIGHT          0x004000
#define STEAM_CONTROLLER_BUTTON_STEAM              0x002000
#define STEAM_CONTROLLER_BUTTON_NAV_LEFT           0x001000
#define STEAM_CONTROLLER_BUTTON_JOYSTICK           0x000040
#define STEAM_CONTROLLER_BUTTON_RIGHT_TOUCH        0x000010
#define STEAM_CONTROLLER_BUTTON_LEFT_TOUCH         0x000008
#define STEAM_CONTROLLER_BUTTON_RIGHT_PAD          0x000004
#define STEAM_CONTROLLER_BUTTON_LEFT_PAD           0x000002
#define STEAM_CONTROLLER_BUTTON_RIGHT_INNER_PADDLE 0x000001

#define STEAM_CONTROLLER_FLAG_REPORT               0x0004
#define STEAM_CONTROLLER_FLAG_BUTTONS              0x0010
#define STEAM_CONTROLLER_FLAG_PADDLES              0x0020
#define STEAM_CONTROLLER_FLAG_JOYSTICK             0x0080
#define STEAM_CONTROLLER_FLAG_LEFT_PAD             0x0100
#define STEAM_CONTROLLER_FLAG_RIGHT_PAD            0x0200


static BLEUUID     hidUUID("00001812-0000-1000-8000-00805f9b34fb"); /* controller announces this service */
static BLEUUID serviceUUID("100F6C32-1735-4313-B402-38567131E5F3"); /* but instead use this one */
static BLEUUID   inputUUID("100F6C33-1735-4313-B402-38567131E5F3"); /* and this characterstic within it to receive the reports */
static BLEUUID    rprtUUID("100F6C34-1735-4313-B402-38567131E5F3"); /* plus this one to configure the controller for sending */
uint8_t startReportCommand[] = { 0xC0, 0x87, 0x03, 0x08, 0x07, 0x00 }; /* command sent to enable report sending. see: https://github.com/haxpor/sdl2-samples/blob/master/android-project/app/src/main/java/org/libsdl/app/HIDDeviceBLESteamController.java */

static boolean doConnect = false;
static boolean ble_connected = false;
static boolean ble_scanning = false;
static BLERemoteCharacteristic* ble_remote_characteristic;
static BLEAdvertisedDevice* ble_remote_device;
static uint32_t ble_last_buttons = 0;

static bool ble_led_on = false;
static bool ble_led_bright = false;
static bool btn_steam = false;

template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

static void parsePacket(uint8_t* buf, size_t len)
{
  int pos = 0;
  
  if(buf[pos] != 0xc0)
  {
    Serial.printf("[SC] unknown reply 0x%02X\n", buf[pos]);
    return;
  }
  pos++;

  if((buf[pos] & 0x0f) == 0x05)
  {
    Serial.print("[SC] Idle: ");
    for(int pos = 0; pos < len; pos++)
    {
      Serial.printf("0x%02X ", buf[pos]);
    }
    Serial.println();
  }
  else if((buf[pos] & 0x0f) == STEAM_CONTROLLER_FLAG_REPORT)
  {
    uint16_t flags = ((buf[pos+1] << 8) | buf[pos]) & ~0x0f;
    
    pos += 2;

    if(flags & STEAM_CONTROLLER_FLAG_BUTTONS)
    {
      uint32_t buttons = (buf[pos+0] << 16) | (buf[pos+1] << 8) | buf[pos+2];
      
 #if 0
      Serial.printf("[SC] Buttons: 0x%08X  ", buttons);
      if(buttons & STEAM_CONTROLLER_BUTTON_A)
      {
        Serial.printf("A, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_X)
      {
        Serial.printf("X, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_B)
      {
        Serial.printf("B, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_Y)
      {
        Serial.printf("Y, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_LEFT_PADDLE)
      {
        Serial.printf("left paddle, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_RIGHT_PADDLE)
      {
        Serial.printf("right paddle, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_LEFT_INNER_PADDLE)
      {
        Serial.printf("left inner paddle, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_RIGHT_INNER_PADDLE)
      {
        Serial.printf("right inner paddle, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_NAV_LEFT)
      {
        Serial.printf("arrow left, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_STEAM)
      {
        Serial.printf("Steam, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_NAV_RIGHT)
      {
        Serial.printf("arrow right, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_LEFT_PAD)
      {
        Serial.printf("left pad, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_RIGHT_PAD)
      {
        Serial.printf("right pad, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_RIGHT_TOUCH)
      {
        Serial.printf("right touch, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_LEFT_TOUCH)
      {
        Serial.printf("left touch, ");
      }
      if(buttons & STEAM_CONTROLLER_BUTTON_JOYSTICK)
      {
        Serial.printf("joy, ");
      }
      Serial.printf("\n");
#endif

      pos += 3;
      flags &= ~STEAM_CONTROLLER_FLAG_BUTTONS;
      
      if(!btn_steam && (buttons & STEAM_CONTROLLER_BUTTON_STEAM))
      {
        lego_init();
        btn_steam = true;
      }
      else if(btn_steam && !(buttons & STEAM_CONTROLLER_BUTTON_STEAM))
      {
        btn_steam = false;
      }
      
      if(buttons & STEAM_CONTROLLER_BUTTON_A)
      {
        ble_led_bright = true;
      }
      else if(ble_last_buttons & STEAM_CONTROLLER_BUTTON_A)
      {
        ble_led_bright = false;
      }

      if(buttons & STEAM_CONTROLLER_BUTTON_B)
      {
        ble_led_on ^= true;
      }
      
      if(btn_steam)
      {
        if(buttons & STEAM_CONTROLLER_BUTTON_A)
        {
          current_config.invert[0] ^= true;
          printf("[CFG] Invert motor A: %s\n", current_config.invert[0] ? "yes" : "no");
          config_modified = true;
        }
        
        if(buttons & STEAM_CONTROLLER_BUTTON_B)
        {
          current_config.invert[1] ^= true;
          printf("[CFG] Invert motor B: %s\n", current_config.invert[1] ? "yes" : "no");
          config_modified = true;
        }
        
        if(buttons & STEAM_CONTROLLER_BUTTON_X)
        {
          current_config.swap ^= true;
          printf("[CFG] Swap motor A/B: %s\n", current_config.swap ? "yes" : "no");
          config_modified = true;
        }
        
        if(buttons & STEAM_CONTROLLER_BUTTON_Y)
        {
          current_config.kinematics ^= true;
          printf("[CFG] Use tank kinematics: %s\n", current_config.kinematics ? "yes" : "no");
          config_modified = true;
        }
        
        if(buttons & STEAM_CONTROLLER_BUTTON_NAV_RIGHT)
        {
          printf("[CFG] Enable WiFi\n");
          wifi_setup();
        }
        
        if(buttons & STEAM_CONTROLLER_BUTTON_NAV_LEFT)
        {
          printf("[CFG] Reboot\n");
          ESP.restart();
        }
      }

      ble_last_buttons = buttons;
    }
    
    if(flags & STEAM_CONTROLLER_FLAG_PADDLES)
    {
      uint8_t left = buf[pos];
      uint8_t right = buf[pos+1];
#if 0
      printf("[SC] FrontPaddle: %d %d\n", left, right);
#endif
      pos+=2;

      if(btn_steam)
      {
        printf("[CFG] set intensity: %d\n", right);
        current_config.light_intensity = right;
      }
      flags &= ~STEAM_CONTROLLER_FLAG_PADDLES;
    }
    
    if(flags & STEAM_CONTROLLER_FLAG_JOYSTICK)
    {
      int16_t joy_x = ((uint16_t)buf[pos+1] << 8 | buf[pos]);
      int16_t joy_y = ((uint16_t)buf[pos+3] << 8 | buf[pos+2]);

      float joy_x_f = (float)joy_x / 32760.0f;
      float joy_y_f = (float)joy_y / 32760.0f;

      if(current_config.kinematics)
      {
        kin_set(joy_x_f, joy_y_f);
      }
      else
      {
        lego_motor(0, joy_x_f);
        lego_motor(1, joy_y_f);
      }
#if 0
      printf("[SC] Joystick: %d %d\n", joy_x, joy_y);
#endif
      pos+=4;
      flags &= ~STEAM_CONTROLLER_FLAG_JOYSTICK;
    }
    
    if(flags & STEAM_CONTROLLER_FLAG_LEFT_PAD)
    {
      int16_t joy_x = (buf[pos+1] << 8 | buf[pos]);
      int16_t joy_y = (buf[pos+3] << 8 | buf[pos+2]);
      
#if 0
      printf("[SC] LeftPad: %d %d\n", joy_x, joy_y);
#endif
      pos+=4;
      flags &= ~STEAM_CONTROLLER_FLAG_LEFT_PAD;
    }
    
    if(flags & STEAM_CONTROLLER_FLAG_RIGHT_PAD)
    {
      int16_t joy_x = (buf[pos+1] << 8 | buf[pos]);
      int16_t joy_y = (buf[pos+3] << 8 | buf[pos+2]);
      
#if 0
      printf("[SC] RightPad: %d %d\n", joy_x, joy_y);
#endif
      pos+=4;
      flags &= ~STEAM_CONTROLLER_FLAG_RIGHT_PAD;
    }

    int ble_led_value = 0;
    if(ble_led_bright)
    {
      ble_led_value = 0xFF;
    }
    else if(ble_led_on)
    {
      ble_led_value = current_config.light_intensity;
    }
    
    lego_led(ble_led_value);
    
    if(flags)
    {
      Serial.print("[SC] still got flags: ");
      for(int pos = 0; pos < len; pos++)
      {
        Serial.printf("0x%02X ", buf[pos]);
      }
      Serial.println();
    }
  }
  else
  {
    Serial.print("[SC] Unknown packet: ");
    for(int pos = 0; pos < len; pos++)
    {
      Serial.printf("0x%02X ", buf[pos]);
    }
    Serial.println();
  }
}

static void report_cbr(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* buf, size_t len, bool isNotify)
{
  parsePacket(buf, len);
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient* pclient) 
  {
  }

  void onDisconnect(BLEClient* pclient)
  {
    ble_connected = false;
  }
};

bool connectToServer()
{
    Serial.printf("[BLE] Connecting to %s\n", ble_remote_device->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    pClient->setClientCallbacks(new MyClientCallback());
    pClient->connect(ble_remote_device);

#if 0
    Serial.println(" - Services: ");
    std::map<std::string, BLERemoteService*> *rmtServices = pClient->getServices(); 

    for (std::map<std::string, BLERemoteService*>::iterator its = rmtServices->begin(); its != rmtServices->end(); ++its)
    {
      Serial.print("   ");
      Serial.print(its->first.c_str());
      Serial.println();
      
      Serial.println("     Characteristics: ");
      std::map<std::string, BLERemoteCharacteristic*>* serviceCharacteristics = its->second->getCharacteristics();
  
      if(!serviceCharacteristics)
      {
        Serial.println("    <<FAILED>> ");
      }
      else
      {
        for (std::map<std::string, BLERemoteCharacteristic*>::iterator itc = serviceCharacteristics->begin(); itc != serviceCharacteristics->end(); ++itc)
        {
          Serial.print("      ");
          Serial.print(itc->first.c_str());
          Serial.print(", ");
          Serial.print(itc->second->canNotify() ? "NOTIFY " : "");
          Serial.print(itc->second->canRead() ? "READ " : "");
          Serial.print(itc->second->canWrite() ? "WRITE " : "");
          Serial.print(itc->second->canWriteNoResponse() ? "WRITE_NO_RESPONSE " : "");
          Serial.println();
        }
      }
      Serial.println();
    }
    Serial.println();
    
#endif

    BLERemoteService* service = pClient->getService(serviceUUID);
    if (service == nullptr)
    {
      Serial.printf("[BLE]   FAILED to find our service UUID: %s\n", serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println("[BLE]   service found");
    
    ble_remote_characteristic = service->getCharacteristic(inputUUID);
    if (ble_remote_characteristic == nullptr)
    {
      Serial.printf("[BLE]   FAILED to find our characteristic UUID: %s\n", inputUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println("[BLE]   characteristic found");


    if(ble_remote_characteristic->canNotify())
    {
      ble_remote_characteristic->registerForNotify(report_cbr);
      Serial.println("[BLE]   characteristic subscribed");
    }

    BLERemoteCharacteristic *reportChar = service->getCharacteristic(rprtUUID);

    if(reportChar && reportChar->canWrite())
    {
      reportChar->writeValue(startReportCommand, sizeof(startReportCommand));
    }
    else
    {
      Serial.println("[BLE]   FAILED to start reporting");
      pClient->disconnect();
      return false;
    }
    
    ble_connected = true;
    return true;
}

class BLENewDevice: public BLEAdvertisedDeviceCallbacks
{
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    bool hasServices = advertisedDevice.haveServiceUUID();
    bool isHid = hasServices && advertisedDevice.isAdvertisingService(hidUUID);
    const char *devName = advertisedDevice.getName().c_str();
    
    Serial.print("[BLE] Device found: ");
    Serial.print(devName);
    Serial.print(hasServices ? " <SERVICES> " : " ");
    Serial.print(isHid ? " <HID> " : " ");
    Serial.println();

    /* found a HID device which advertises as SteamController */
    if (isHid && !strcmp(devName, "SteamController"))
    {
      Serial.printf("[BLE] connecting to '%s'\n", devName);
      BLEDevice::getScan()->stop();
      ble_remote_device = new BLEAdvertisedDevice(advertisedDevice);
      ble_scanning = false;
      doConnect = true;
    }
  }
};

void ble_scan_complete (BLEScanResults results)
{
  Serial.println("[BLE] Scan finished");
  ble_scanning = false;
}

void ble_setup()
{
  Serial.println("[BLE] initializing");
  BLEDevice::init("");

  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new BLENewDevice());
  Serial.println("[BLE] start intial scan");
  pBLEScan->start(5, &ble_scan_complete, false);
  
  ble_scanning = true;
}

bool ble_loop()
{
  bool busy = false;
  
  if(doConnect)
  {
    doConnect = false;
    
    if(connectToServer())
    {
      Serial.println("[BLE] Connected");
      led_anim_blue(led_anim_ble_connected);
    }
    else
    {
      Serial.println("[BLE] FAILED to connect");
      led_anim_blue(led_anim_emergency);
    }
  }

  if(!ble_connected)
  {
    if(!ble_scanning)
    {
      Serial.println("[BLE] Restart scan");
      BLEDevice::getScan()->start(5, &ble_scan_complete, false);
      ble_scanning = true;
    }
    led_anim_blue(led_anim_shortdoubleblinkloop);
  }
  
  return busy;
}

