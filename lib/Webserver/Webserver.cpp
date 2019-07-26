#include <FS.h>
#include "Webserver.h"
#include <SettingsManager.h>

Webserver::Webserver() : webserver(80) {}

bool Webserver::begin()
{
    webserver.serveStatic("/", SPIFFS, "/web/")
        .setDefaultFile("index.html")
        .setTemplateProcessor(std::bind(&Webserver::indexProcessor, this, std::placeholders::_1));

    webserver.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });

    webserver.begin();

    return true;
}

String Webserver::indexProcessor(const String &key)
{
    if (key == "hostname")
    {
        return SettingsManager::get().host;
    }

    return "";
}

void Webserver::loop()
{
}
