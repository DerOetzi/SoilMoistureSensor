#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Sensors.h"
#include "ESPAsyncWebServer.h"

class Webserver
{
public:
    Webserver(void);
    bool begin();
    void loop(SensorsData &data);

private:
    AsyncWebServer webserver;
    SensorsData _data;

    void handleRoot(AsyncWebServerRequest *request);
    String processorRoot(const String &key);
    void stateJson(AsyncWebServerRequest *request);

    void handleConfiguration(AsyncWebServerRequest *request);
    String processorConfiguration(const String &key);
};

#endif