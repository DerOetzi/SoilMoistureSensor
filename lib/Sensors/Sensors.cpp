#include "Sensors.h"
#include <Settings.h>

Sensors::Sensors() {}

bool Sensors::begin()
{
    if (_bme_sensor.begin())
    {
        Serial.println("BME280 sensor detected");
        _bme_presence = true;
        _bme_sensor.setTempOffset(Settings.temp_offset);
        _bme_sensor.setAltitude(Settings.altitude);
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
        _data.soil_percent = round(((Settings.dry - _data.soil_voltage) * 100) / (Settings.dry - Settings.wet));

        if (_bme_presence)
        {
            _data.bme_presence = true;
            _bme_sensor.readSensor();
            _data.temperature = round(_bme_sensor.getTemperature() * 10) / 10;
            _data.humidity = round(_bme_sensor.getHumidity());
            _data.pressure = round(_bme_sensor.getPressure() * 10) / 10;
            if (Settings.altitude > 0)
            {
                _data.altitude = Settings.altitude;
                _data.pressureSeaLevel = round(_bme_sensor.getPressureSeaLevel() * 10) / 10;
            }
        }
    }
}

SensorsData Sensors::data()
{
    return _data;
}

void Sensors::serialprint()
{
}
