
#ifndef __CONFIG_H__
#define __CONFIG_H__

typedef struct
{
  uint64_t magic1;
  uint32_t version;
  
  bool invert[2];
  bool swap;
  bool kinematics;
  uint8_t light_intensity;
  
  /* footer */
  uint64_t magic2;
} config_t;

#define CONFIG_VERSION 2

extern config_t current_config;
extern bool config_modified;

#endif

