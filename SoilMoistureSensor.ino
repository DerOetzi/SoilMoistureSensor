#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <PubSubClient.h>

#include <NTPClient.h>

#include <DoubleResetDetector.h>

#include "bme280.h"

#include "settings.h"

#define DRD_TIMEOUT 10
#define DRD_ADDRESS 0
#define ANALOG_PIN A0
#define CONFIG_PORTAL_TIMEOUT_SECONDS 10

bool connected = false;
WiFiClient espClient;
PubSubClient mqttClient(espClient);
bool mqtt_enabled = false;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);

bool bme_presence = false;
BME280 bmeSensor;

long now;

void configModeCallback(WiFiManager *myWifiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
 
  drd.stop();  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount FS");
    return;
  }

  initSettings();
  loadSettings();

  wifi_station_set_hostname(Settings.my_hostname);

  WiFiManager wifiManager;
  wifiManager.setDebugOutput(false);
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_SECONDS);

  if (drd.detectDoubleReset()) {
    Serial.println("Double Reset Detected");
    wifiManager.startConfigPortal(Settings.my_hostname, AP_PASSWORD);
  } else if (wifiManager.autoConnect(Settings.my_hostname, AP_PASSWORD)) {
    connected = true;
  } 

  if (connected) {
    Serial.print("connected...");
    Serial.println(WiFi.localIP());

    webserver_setup();
    
    if(strcmp(Settings.mqtt_host, "") > 0) {
      mqtt_enabled = true;
      Serial.println("MQTT activated");
      mqttClient.setServer(Settings.mqtt_host, Settings.mqtt_port);
    } 
  
    timeClient.begin();
  }

  if (bmeSensor.begin()) {
    Serial.println("BME280 sensor detected"); 
    bmeSensor.setTempOffset(Settings.temp_offset); 
    bmeSensor.setAltitude(Settings.altitude);
    bme_presence = true;
  } else {
    Serial.println("BME280 sensor not detected");  
  }

  drd.stop();
}

long lastRetry = 0;
void reconnect() {
  if (!mqtt_enabled) {
    return;  
  }
  
  if (mqttClient.connected()) {
    mqttClient.loop();
  } else {
    if (now - lastRetry > 10000) {
      lastRetry = now;
      Serial.print("Attempting MQTT connection...");
      if (mqttClient.connect(Settings.mqtt_client, Settings.mqtt_user, Settings.mqtt_pwd)) {
        Serial.println("connected");
        mqttClient.loop();
      } else {
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" Retry in 10s");
      }
    }
  }
}

int soil_voltage(NAN);
int soil_percent(NAN);
float temperature(NAN);
int humidity(NAN);
float pressure(NAN);
float pressureSeaLevel(NAN);

void loop() {
  if (Settings.restart_flag == 1) {
    Serial.println("Restart as requested");
    ESP.reset();  
  }

  now = millis();
  readSensor();  

  if (connected) {
    timeClient.update();
    webserver_loop();
    reconnect();  
    publishMQTT();
  }
  
  delay(250);
}

long lastRead = 0;
void readSensor() {
  if (now - lastRead > 10000) {
    lastRead = now;
    soil_voltage = analogRead(ANALOG_PIN);
  
    Serial.print("Soil Voltage: ");
    Serial.print(soil_voltage);
    Serial.print(" mV");

    soil_percent = ((Settings.dry - soil_voltage) * 100) / (Settings.dry - Settings.wet);
    soil_percent = round(soil_percent);
  
    Serial.print(", Soil Percent: ");
    Serial.print(soil_percent);
    Serial.print(" %");

    if (bme_presence) {
      bmeSensor.readSensor();
      temperature = round(bmeSensor.getTemperature() * 10) / 10;
      humidity = round(bmeSensor.getHumidity());
      pressure = round(bmeSensor.getPressure() * 10) / 10;
      
      Serial.print(", Temperature: ");
      Serial.print(temperature);
      Serial.print(" °C, Humidity: ");
      Serial.print(humidity);
      Serial.print(" %, Pressure: ");
      Serial.print(pressure);
      if (Settings.altitude > 0) {
        pressureSeaLevel = round(bmeSensor.getPressureSeaLevel() * 10) / 10;
        Serial.print(" mbar, Pressure N.N.: ");
        Serial.print(pressureSeaLevel);
      }
      Serial.print(" mbar");
    }

    Serial.println();
  }   
}

long lastMsg = 0;
void publishMQTT() {
  if (!(mqtt_enabled && mqttClient.connected())) {
    return;  
  }
  
  if (now - lastMsg > 60000) {
    lastMsg = now;
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
  
    json["ts"] = timeClient.getFormattedTime();
    json["v"] = soil_voltage;
    json["s"] = soil_percent;

    if (bme_presence) {
      json["t"] = temperature;
      json["h"] = humidity;
      json["p"] = pressure; 
      if (Settings.altitude > 0) {
        json["pNN"] = pressureSeaLevel;  
      }   
    }
  
    char msg[512];
    char outMsg[512];
  
    json.printTo(outMsg, sizeof(outMsg));
    snprintf (msg,1000, "%s",outMsg);
    Serial.println(Settings.mqtt_topic);
    Serial.println(msg);
    mqttClient.publish(Settings.mqtt_topic, msg);  
  }
}
