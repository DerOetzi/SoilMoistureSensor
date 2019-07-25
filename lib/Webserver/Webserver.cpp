#include "Webserver.h"

Webserver::Webserver() {}

bool Webserver::begin(const char *hostname)
{
    strcpy(_hostname, hostname);

    server.reset(new AsyncWebServer(80));

    server->serveStatic("/", SPIFFS, "/web/");

    server->begin();

    return true;
}

String Webserver::processor(const String &var)
{
    return String();
}