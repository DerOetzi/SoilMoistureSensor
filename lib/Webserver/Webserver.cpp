#include <FS.h>
#include "Webserver.h"

Webserver::Webserver() {}

bool Webserver::begin(const char *hostname)
{
    strcpy(_hostname, hostname);

    server.reset(new ESP8266WebServer(80));

    server->on("/", std::bind(&Webserver::handleRoot, this));

    server->onNotFound(std::bind(&Webserver::handleServeStatic, this));

    server->begin();

    return true;
}

void Webserver::loop()
{
    server->handleClient();
}

void Webserver::handleRoot()
{
}

const char *rootProcessor(const String &key)
{
    Serial.println(key);
    return "Hallo Welt!";
}

void Webserver::handleServeStatic()
{
    String path = server->uri();
    if (path.endsWith("/"))
    {
        path += "index.html";
    }

    path = "/web" + path;

    if (SPIFFS.exists(path))
    {
        String contentType = getContentType(path);
        File file = SPIFFS.open(path, "r");
        server->streamFile(file, contentType);
        file.close();
    }
    else
    {
        server->send(404, "text/plain", "404: Not Found");
    }
}

String Webserver::getContentType(String filename)
{
    if (filename.endsWith(".html"))
    {
        return "text/html";
    }
    else if (filename.endsWith(".css"))
    {
        return "text/css";
    }
    else if (filename.endsWith(".js"))
    {
        return "application/javascript";
    }
    else if (filename.endsWith(".ico"))
    {
        return "image/x-icon";
    }
    return "text/plain";
}
