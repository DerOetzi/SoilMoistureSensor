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
    SensorsData data(void);
    void serialprint(void);
    

private:
    BME280 _bme_sensor;
    bool _bme_presence = false;

    SensorsData _data = {0, 0, 0, false, 0.0, 0, 0.0, 0, 0.0};
};

#endif