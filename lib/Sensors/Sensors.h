#ifndef SENSORS_H
#define SENSORS_H

#include "Arduino.h"
#include "BME280.h"

struct SensorsData
{
public:
    int timestamp;
    int soil_voltage;
    int soil_percent;
    bool bme_presence;
    float temperature;
    int humidity;
    float pressure;
    int altitude;
    float pressureSeaLevel;
};

class Sensors
{
public:
    Sensors(void);
    bool begin();
    void read(void);
    SensorsData &data(void)
    {
        return _data;
    }
    void serialprint(void);

private:
    SensorsData _data = {
        0,     // int timestamp;
        0,     // int soil_voltage;
        0,     // int soil_percent;
        false, // bool bme_presence;
        0.0,   // float temperature;
        0,     // int humidity;
        0.0,   // float pressure;
        0,     // int altitude;
        0.0    // float pressureSeaLevel;
    };

    BME280 _bme_sensor;
    bool _bme_presence = false;
};

#endif