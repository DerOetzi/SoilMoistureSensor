#ifndef WIFICONNECTOR_H
#define WIFICONNECTOR_H

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <SettingsManager.h>

#define DNS_PORT 53

class WiFiConnector
{
public:
    WiFiConnector(void);

    bool begin(const char *ap_name, const char *ap_pwd);
    int connect(String ssid, String pwd);
    void loop(void);

private:
    std::unique_ptr<DNSServer> dnsServer;
    bool ap_active = false;

    bool accesspoint(const char *ap_name, const char *ap_pwd);
};

#endif