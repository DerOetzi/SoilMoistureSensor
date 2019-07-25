#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Sensors.h"
#include "ESPAsyncWebServer.h"

class Webserver
{
public:
    Webserver(void);
    bool begin(const char *hostname);
    void loop(SensorsData data);

private:
    char _hostname[40];
    std::unique_ptr<AsyncWebServer> server;
    String processor(const String &var);
};

#endif