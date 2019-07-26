#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Sensors.h"
#include <ESP8266WebServer.h>

class Webserver
{
public:
    Webserver(void);
    bool begin(const char *hostname);
    void loop();

private:
    char _hostname[40];
    std::unique_ptr<ESP8266WebServer> server;

    void handleRoot();

    void handleServeStatic(void);
    String getContentType(String filename);
};

#endif