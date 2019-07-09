#include <FS.h>
#include <Arduino.h>

#include <SettingsManager.h>
#include <Sensors.h>
#include <WiFiConnector.h>

Sensors sensors;
WiFiConnector wifi;

void setup()
{
    Serial.begin(115200);
    Serial.println();
    SPIFFS.begin();

    SettingsManager::instance().load();

    sensors.begin();

    wifi.begin(SettingsManager::get().my_hostname, AP_PASSWORD);
}

void loop()
{
    sensors.read();
    wifi.loop();

    delay(250);
}