#include <FS.h>
#include <ArduinoJson.h>

#include "Webserver.h"
#include <SettingsManager.h>

Webserver::Webserver() : webserver(80) {}

bool Webserver::begin()
{
    webserver.on("/", HTTP_GET, std::bind(&Webserver::handleRoot, this, std::placeholders::_1));
    webserver.on("/rest/states", HTTP_GET, std::bind(&Webserver::stateJson, this, std::placeholders::_1));

    webserver.on("/config", HTTP_GET, std::bind(&Webserver::handleConfiguration, this, std::placeholders::_1));
    
    webserver.serveStatic("/", SPIFFS, "/web/")
        .setDefaultFile("index.html");

    webserver.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });

    webserver.begin();

    return true;
}

void Webserver::loop(SensorsData &data)
{
    _data = data;
}

void Webserver::handleRoot(AsyncWebServerRequest *request)
{
    request->send(SPIFFS, "/web/index.html", String(), false, std::bind(&Webserver::processorRoot, this, std::placeholders::_1));
}

String Webserver::processorRoot(const String &key)
{
    if (key == "hostname")
    {
        return SettingsManager::get().host;
    }
    else if (key == "bme_presence" && !_data.bme_presence)
    {
        return "nobme280";
    }
    else if (key == "altitude")
    {
        return String(_data.altitude);
    }

    return "";
}
void Webserver::stateJson(AsyncWebServerRequest *request)
{
    StaticJsonDocument<512> json;
    json["ts"] = _data.timestamp;
    json["sv"] = _data.soil_voltage;
    json["sp"] = _data.soil_percent;

    if (_data.bme_presence)
    {
        json["t"] = _data.temperature;
        json["h"] = _data.humidity;
        json["p"] = _data.pressure;

        if (_data.altitude > 0)
        {
            json["psl"] = _data.pressureSeaLevel;
        }
    }

    String output;
    serializeJson(json, output);

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", output);
    request->send(response);
}

void Webserver::handleConfiguration(AsyncWebServerRequest *request)
{
    request->send(SPIFFS, "/web/configuration.html", String(), false, std::bind(&Webserver::processorConfiguration, this, std::placeholders::_1));
}

String Webserver::processorConfiguration(const String &key)
{
    if (key == "hostname" || key == "host")
        return SettingsManager::get().host;
    else if (key == "bme_presence" && !_data.bme_presence)
        return "nobme280";
    else if (key == "altitude")
        return String(SettingsManager::get().altitude);
    else if (key == "timezone")
        return String(SettingsManager::get().timezone);
    else if (key == "temp_offset")
        return String(SettingsManager::get().temp_offset);
    else if (key == "dry")
        return String(SettingsManager::get().dry);
    else if (key == "wet")
        return String(SettingsManager::get().wet);
    else if (key == "mqtt_host")
        return SettingsManager::get().mqtt_host;
    else if (key == "mqtt_port")
        return String(SettingsManager::get().mqtt_port);
    else if (key == "mqtt_client")
        return SettingsManager::get().mqtt_client;
    else if (key == "mqtt_user")
        return SettingsManager::get().mqtt_user;
    else if (key == "mqtt_topic")
        return SettingsManager::get().mqtt_topic;

    return "";
}