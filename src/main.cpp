#include <FS.h>
#include <Arduino.h>

#include <SettingsManager.h>
#include <Sensors.h>
#include <WiFiConnector.h>
#include <Webserver.h>

Sensors sensors;
WiFiConnector wifi;
Webserver web;

void setup()
{
    Serial.begin(115200);
    Serial.println();
    SPIFFS.begin();

    SettingsManager::instance().load();

    sensors.begin();

    wifi.begin(SettingsManager::get().my_hostname, AP_PASSWORD);

    web.begin(SettingsManager::get().host);
}

void loop()
{
    sensors.read();
    wifi.loop();

    web.loop();

    delay(250);
}