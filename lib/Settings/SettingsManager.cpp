#include "SettingsManager.h"

#include <Arduino.h>
#include <cstring>

bool SettingsManager::load()
{
    SettingsManager::init();
    return true;
}

bool SettingsManager::save()
{
    return true;
}

void SettingsManager::init()
{
    Settings.restart_flag = 0;
    strcpy(Settings.host, HOSTNAME_DEFAULT);
    Settings.timezone = DEFAULT_TIMEZONE;
    Settings.altitude = DEFAULT_ALTITUDE;

    Settings.temp_offset = DEFAULT_TEMP_OFFSET;
    Settings.dry = DRY_INIT;
    Settings.wet = WET_INIT;

    strcpy(Settings.mqtt_host, "");
    Settings.mqtt_port = MQTT_PORT;
    strcpy(Settings.mqtt_client, "");
    strcpy(Settings.mqtt_user, "");
    strcpy(Settings.mqtt_pwd, "");
    strcpy(Settings.mqtt_topic, MQTT_TOPIC);

    snprintf_P(Settings.my_hostname, sizeof(Settings.my_hostname) - 1, HOSTNAME_PATTERN, HOSTNAME_DEFAULT, ESP.getChipId() & 0x1FFF);
}