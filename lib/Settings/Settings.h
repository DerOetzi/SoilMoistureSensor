#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

#define TRACE 0
#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4

#define HOSTNAME_PATTERN "%s-%04d"
#define HOSTNAME_DEFAULT "SoilMoistureSensor"
#define AP_PASSWORD "soilmoisture"
#define DRY_INIT 630
#define WET_INIT 270
#define DEFAULT_TIMEZONE 0
#define DEFAULT_TEMP_OFFSET -5.0
#define DEFAULT_ALTITUDE 0
#define MQTT_PORT 1883
#define MQTT_TOPIC "sensor/soilmoisture"
#define DEFAULT_LOGLEVEL DEBUG

struct Settings
{
public:
  char host[40];
  char my_hostname[40];
  int8_t restart_flag;
  int8_t timezone;
  uint16_t altitude;
  char mqtt_host[33];
  uint16_t mqtt_port;
  char mqtt_client[33];
  char mqtt_user[33];
  char mqtt_pwd[33];
  char mqtt_topic[33];
  float temp_offset;
  uint16_t dry;
  uint16_t wet;
  int8_t loglevel;
};

#endif