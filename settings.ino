void initSettings() {
  Settings.restart_flag = 0;
  strcpy(Settings.host, HOSTNAME_DEFAULT);
  Settings.timezone = DEFAULT_TIMEZONE;
  Settings.altitude = DEFAULT_ALTITUDE;
 
  Settings.temp_offset = DEFAULT_TEMP_OFFSET;
  Settings.dry = DRY_INIT;
  Settings.wet = WET_INIT;

  strcpy(Settings.mqtt_host, "");
  Settings.mqtt_port = MQTT_PORT;  
  strcpy(Settings.mqtt_client, "");
  strcpy(Settings.mqtt_user, "");
  strcpy(Settings.mqtt_pwd, "");
  strcpy(Settings.mqtt_topic, MQTT_TOPIC);
  
  snprintf_P(Settings.my_hostname, sizeof(Settings.my_hostname)-1, HOSTNAME_PATTERN, HOSTNAME_DEFAULT, ESP.getChipId() & 0x1FFF);
}

bool loadSettings() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    configFile.close();
    return false;
  }

  std::unique_ptr<char[]> buf(new char[size]);

  configFile.readBytes(buf.get(), size);
  configFile.close();

  StaticJsonBuffer<512> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  strcpy(Settings.host, json["host"]);
  Settings.timezone = json["timezone"];

  Settings.temp_offset = json["temp_offset"];
  if (json.containsKey("altitude")) {
    Settings.altitude = json["altitude"];
  }
  if (json.containsKey("dry")) {
    Settings.dry = json["dry"];
  }
  if (json.containsKey("wet")) {
    Settings.wet = json["wet"];
  }
  
  if (json.containsKey("mqtt_host")) {
    strcpy(Settings.mqtt_host, json["mqtt_host"]);
  }
  if (json.containsKey("mqtt_port")) {
    Settings.mqtt_port = json["mqtt_port"];
  }
  if (json.containsKey("mqtt_client")) {
    strcpy(Settings.mqtt_client, json["mqtt_client"]);
  }
  if (json.containsKey("mqtt_user")) {
    strcpy(Settings.mqtt_user, json["mqtt_user"]);
  }
  if (json.containsKey("mqtt_pwd")) {
    strcpy(Settings.mqtt_pwd, json["mqtt_pwd"]);
  }
  if (json.containsKey("mqtt_topic")) {
    strcpy(Settings.mqtt_topic, json["mqtt_topic"]);
  }

  return true; 
}

bool saveSettings() {
StaticJsonBuffer<512> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["host"] = Settings.host;
  json["timezone"] = Settings.timezone;
  
  json["temp_offset"] = Settings.temp_offset;
  json["altitude"] = Settings.altitude;
  json["dry"] = Settings.dry;
  json["wet"] = Settings.wet;

  json["mqtt_host"] = Settings.mqtt_host;
  json["mqtt_port"] = Settings.mqtt_port;
  json["mqtt_client"] = Settings.mqtt_client;
  json["mqtt_user"] = Settings.mqtt_user;
  json["mqtt_pwd"] = Settings.mqtt_pwd;
  json["mqtt_topic"] = Settings.mqtt_topic;
  
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    configFile.close();
    return false;
  }

  json.printTo(configFile);
  configFile.close();
  return true;
}
