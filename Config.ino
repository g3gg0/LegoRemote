
#include "Config.h"

config_t current_config;
bool config_modified = false;


void config_setup()
{
  File file = SPIFFS.open("/config.bin", "r");
  
  if(!file)
  {
    Serial.println("[E] failed to open file for reading");
  }
  else
  {
    file.read((uint8_t *)&current_config, sizeof(current_config));
    file.close();
    Serial.println("[i] read status from SPIFFS");
  }
  
  if(current_config.magic1 != 0xDEADBEEF || current_config.magic2 != 0x55AA55AA || current_config.version != CONFIG_VERSION)
  {
    Serial.println("[i] incorrect magics or version, reinit status");
    
    memset((void *)&current_config, 0x00, sizeof(current_config));
  
    current_config.magic1 = 0xDEADBEEF;
    current_config.magic2 = 0x55AA55AA;
    current_config.version = CONFIG_VERSION;

    /* set default config */
    current_config.swap = false;
    current_config.kinematics = true;
    current_config.invert[0] = false;
    current_config.invert[1] = false;
    current_config.light_intensity = 32;
    
    config_modified = true;
  }
}

bool config_loop()
{
  int curTime = millis();
  static int nextTimeSave = 0x7FFFFFFF;

  if(config_modified)
  {
    Serial.println("[i] config modified");
    nextTimeSave = curTime + 5000;
    config_modified = false;
  }

  if(nextTimeSave <= curTime)
  {
    File file = SPIFFS.open("/config.bin", "w");
    if(!file)
    {
      Serial.println("[E] failed to open file for writing");
    }
    else
    {
      file.write((const uint8_t *)&current_config, sizeof(current_config));
      file.close();
      Serial.println("[i] saved config");
    }
    nextTimeSave = 0x7FFFFFFF;
    led_anim_red(led_anim_shortdoubleblink);
  }
}


