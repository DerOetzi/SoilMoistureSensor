#include "Sensors.h"
#include <SettingsManager.h>
#include <string>

Sensors::Sensors() {}

bool Sensors::begin()
{
    if (_bme_sensor.begin())
    {
        Serial.println("BME280 sensor detected");
        _bme_presence = true;
        _bme_sensor.setTempOffset(SettingsManager::get().temp_offset);
        _bme_sensor.setAltitude(SettingsManager::get().altitude);
    }
    else
    {
        Serial.println("BME280 sensor not detected");
    }

    return true;
}

void Sensors::read()
{
    if (millis() - _data.timestamp > 10000)
    {
        _data.timestamp = millis();
        _data.soil_voltage = analogRead(PIN_A0);

        int dry = SettingsManager::get().dry;
        int wet = SettingsManager::get().wet;
        _data.soil_percent = round(((dry - _data.soil_voltage) * 100) / (dry - wet));

        if (_bme_presence)
        {
            _data.bme_presence = true;
            _bme_sensor.readSensor();
            _data.temperature = round(_bme_sensor.getTemperature() * 10) / 10;
            _data.humidity = round(_bme_sensor.getHumidity());
            _data.pressure = round(_bme_sensor.getPressure() * 10) / 10;

            int altitude = SettingsManager::get().altitude;
            if (altitude > 0)
            {
                _data.altitude = altitude;
                _data.pressureSeaLevel = round(_bme_sensor.getPressureSeaLevel() * 10) / 10;
            }
        }

        if (SettingsManager::get().loglevel <= DEBUG)
        {
            serialprint();
        }
    }
}

void Sensors::serialprint()
{
    String output = "Soil Moisture Voltage: ";
    output += _data.soil_voltage;
    output += " mV, Soil Moisture Percent: ";
    output += _data.soil_percent;
    output += " %";

    if (_bme_presence)
    {
        output += ", Temperature: ";
        output += _data.temperature;
        output += " °C, Humidity:";
        output += _data.humidity;
        output += " %, Pressure: ";
        output += _data.pressure;
        if (SettingsManager::get().altitude > 0)
        {
            output += " mbar, Pressure Sealevel: ";
            output += _data.pressureSeaLevel;
        }
        output += " mbar";
    }

    Serial.println(output);
}
