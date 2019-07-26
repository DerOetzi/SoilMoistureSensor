#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Sensors.h"
#include "ESPAsyncWebServer.h"

class Webserver
{
public:
    Webserver(void);
    bool begin();
    void loop();

    String indexProcessor(const String &key);

private:
    AsyncWebServer webserver;
};

#endif