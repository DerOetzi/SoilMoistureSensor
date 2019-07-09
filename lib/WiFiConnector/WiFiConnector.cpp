#include "WiFiConnector.h"

WiFiConnector::WiFiConnector() {}

bool WiFiConnector::begin(char const *ap_name, char const *ap_pwd)
{
    WiFi.mode(WIFI_STA);

    unsigned long startedAt = millis();
    while (millis() - startedAt < 10000)
    {
        delay(100);
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
    }

    return accesspoint(ap_name, ap_pwd);
}

bool WiFiConnector::accesspoint(const char *ap_name, const char *ap_pwd)
{
    int connRes = WiFi.waitForConnectResult();
    if (connRes == WL_CONNECTED)
    {
        WiFi.mode(WIFI_AP_STA);
    }
    else
    {
        WiFi.mode(WIFI_AP);
    }

    if (WiFi.getAutoConnect() == 0)
    {
        WiFi.setAutoConnect(1);
    }

    WiFi.softAP(ap_name, ap_pwd);
    delay(500);

    if (SettingsManager::get().loglevel <= DEBUG)
    {
        Serial.print("Access Point IP: ");
        Serial.println(WiFi.softAPIP());
    }

    dnsServer.reset(new DNSServer());
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer->start(DNS_PORT, "*", WiFi.softAPIP());

    ap_active = true;

    return true;
}

void WiFiConnector::loop()
{
    if (ap_active)
    {
        dnsServer->processNextRequest();
    }
}