#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <Settings.h>

class SettingsManager
{
public:
    static SettingsManager &instance()
    {
        static SettingsManager instance;
        return instance;
    }

    static Settings &get()
    {
        return SettingsManager::instance()._settings;
    }

    bool load(void);
    bool save(void);

private:
    Settings _settings = {
        HOSTNAME_DEFAULT,    //char host[40];
        "",                  //char my_hostname[40];
        0,                   //int8_t restart_flag;
        DEFAULT_TIMEZONE,    //int8_t timezone;
        DEFAULT_ALTITUDE,    //uint16_t altitude;
        "",                  //char mqtt_host[33];
        MQTT_PORT,           //uint16_t mqtt_port;
        "",                  //char mqtt_client[33];
        "",                  //char mqtt_user[33];
        "",                  //char mqtt_pwd[33];
        MQTT_TOPIC,          //char mqtt_topic[33];
        DEFAULT_TEMP_OFFSET, //float temp_offset;
        DRY_INIT,            //uint16_t dry;
        WET_INIT,            //uint16_t wet;
        DEFAULT_LOGLEVEL     //int8_t loglevel
    };

    SettingsManager(){};
    SettingsManager(const SettingsManager &);
    SettingsManager &operator=(const SettingsManager &);
};

#endif