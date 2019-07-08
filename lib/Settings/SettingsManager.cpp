#include "SettingsManager.h"

#include <FS.h>
#include <Arduino.h>
#include <cstring>

bool SettingsManager::load()
{
    snprintf_P(_settings.my_hostname, sizeof(_settings.my_hostname) - 1, HOSTNAME_PATTERN, HOSTNAME_DEFAULT, ESP.getChipId() & 0x1FFF);

    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println("Failed to open config file");
        return false;
    }

    StaticJsonDocument<512> json;
    DeserializationError error = deserializeJson(json, configFile);

    configFile.close();

    if (error)
    {
        Serial.println("Could not parse config-file");
        return false;
    }

    intValue(json, FILE_LOGLEVEL, &_settings.loglevel);
    charValue(json, FILE_HOST, _settings.host);
    intValue(json, FILE_TIMEZONE, &_settings.timezone);

    floatValue(json, FILE_TEMP_OFFSET, &_settings.temp_offset);
    intValue(json, FILE_ALTITUDE, &_settings.altitude);
    intValue(json, FILE_DRY, &_settings.dry);
    intValue(json, FILE_WET, &_settings.wet);

    charValue(json, FILE_MQTT_HOST, _settings.mqtt_host);
    intValue(json, FILE_MQTT_PORT, &_settings.mqtt_port);
    charValue(json, FILE_MQTT_CLIENT, _settings.mqtt_client);
    charValue(json, FILE_MQTT_USER, _settings.mqtt_user);
    charValue(json, FILE_MQTT_PWD, _settings.mqtt_pwd);
    charValue(json, FILE_MQTT_TOPIC, _settings.mqtt_topic);

    return true;
}

void SettingsManager::intValue(StaticJsonDocument<512> json, const char *key, int *out)
{
    if (json.containsKey(key))
    {
        *out = json[key];
    }
}

void SettingsManager::floatValue(StaticJsonDocument<512> json, const char *key, float *out)
{
    if (json.containsKey(key))
    {
        *out = json[key];
    }
}

void SettingsManager::charValue(StaticJsonDocument<512> json, const char *key, char *out)
{
    if (json.containsKey(key))
    {
        strcpy(out, json[key]);
    }
}

bool SettingsManager::save()
{
    StaticJsonDocument<512> json;
    json[FILE_HOST] = _settings.host;
    json[FILE_TIMEZONE] = _settings.timezone;

    json[FILE_TEMP_OFFSET] = _settings.temp_offset;
    json[FILE_ALTITUDE] = _settings.altitude;
    json[FILE_DRY] = _settings.dry;
    json[FILE_WET] = _settings.wet;

    json[FILE_MQTT_HOST] = _settings.mqtt_host;
    json[FILE_MQTT_PORT] = _settings.mqtt_port;
    json[FILE_MQTT_CLIENT] = _settings.mqtt_client;
    json[FILE_MQTT_USER] = _settings.mqtt_user;
    json[FILE_MQTT_PWD] = _settings.mqtt_pwd;
    json[FILE_MQTT_TOPIC] = _settings.mqtt_topic;

    json[FILE_LOGLEVEL] = _settings.loglevel;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println("Failed to open config file for writing");
        configFile.close();
        return false;
    }

    if (serializeJson(json, configFile) == 0)
    {
        Serial.println("Failed to write to file");
    }

    configFile.close();

    return true;
}