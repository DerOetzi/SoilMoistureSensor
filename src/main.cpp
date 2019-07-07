#include <Arduino.h>

#include <SettingsManager.h>
#include <Sensors.h>

Sensors sensors;

void setup() {
    Serial.begin(115200);
    Serial.println();
    
    SettingsManager::load();

    sensors.begin();
}

void loop() {
    sensors.read();
}