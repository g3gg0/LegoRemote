


const char *ssid = "x";
const char *password = "x";
bool connecting = false;
bool wifi_enabled = false;


void wifi_setup()
{
  Serial.printf("[WiFi] Connecting...\n");
  WiFi.begin(ssid, password);
  connecting = true;
  wifi_enabled = true;
}

bool wifi_loop(void)
{
  int status = WiFi.status();
  int curTime = millis();
  static int nextTime = 0;
  static int stateCounter = 0;

  if(!wifi_enabled)
  {
    return false;
  }

  if(nextTime > curTime)
  {
    return false;
  }

  /* standard refresh time */
  nextTime = curTime + 100;
  
  switch(status)
  {
    case WL_CONNECTED:
      if(connecting)
      {
        connecting = false;
        Serial.print("[WiFi] Connected, IP address: ");
        Serial.println(WiFi.localIP());
        ota_setup();
      }
      break;

    case WL_CONNECTION_LOST:
      Serial.printf("[WiFi] Connection lost\n");
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      nextTime = curTime + 500;
      break;

    case WL_CONNECT_FAILED:
      Serial.printf("[WiFi] Connection failed\n");
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      nextTime = curTime + 1000;
      break;

    case WL_NO_SSID_AVAIL:
      Serial.printf("[WiFi] No SSID\n");
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      nextTime = curTime + 2000;
      break;

    case WL_SCAN_COMPLETED:
      Serial.printf("[WiFi] Scan completed\n");
      connecting = false;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      break;

    case WL_DISCONNECTED:
      if(!connecting)
      {
        Serial.printf("[WiFi] Disconnected\n");
        connecting = false;
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        break;
      }
      else
      {
        if(++stateCounter > 50)
        {
          Serial.printf("[WiFi] Timeout, aborting\n");
          connecting = false;
          WiFi.disconnect();
          WiFi.mode(WIFI_OFF);
        }
      }

    case WL_IDLE_STATUS:
      if(!connecting)
      {
        connecting = true;
        Serial.printf("[WiFi]  Idle, connecting to %s\n", ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        stateCounter = 0;
        break;
      }
      
    case WL_NO_SHIELD:
      if(!connecting)
      {
        connecting = true;
        Serial.printf("[WiFi]  Disabled (%d), connecting to %s\n", status, ssid);
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        stateCounter = 0;
        break;
      }
  }
    
  return false;
}


