void initSettings() {
  Settings.restart_flag = 0;
  strcpy(Settings.host, HOSTNAME_DEFAULT);
  Settings.timezone = DEFAULT_TIMEZONE;
  Settings.mqtt_port = MQTT_PORT;
  strcpy(Settings.mqtt_topic, MQTT_TOPIC);  
  Settings.dry = DRY_INIT;
  Settings.wet = WET_INIT;
  Settings.temp_offset = DEFAULT_TEMP_OFFSET;
 
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

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(buf.get());

  if (!json.success()) {
    Serial.println("Failed to parse config file");
    return false;
  }

  strcpy(Settings.host, json["host"]);
  Settings.timezone = json["timezone"];
  
  Settings.temp_offset = json["temp_offset"];
  if (json.containsKey("dry")) {
    Settings.dry = json["dry"];
  }
  if (json.containsKey("wet")) {
    Settings.wet = json["wet"];
  }
  
  return true; 
}

bool saveSettings() {
StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["host"] = Settings.host;
  json["timezone"] = Settings.timezone;
  
  json["temp_offset"] = Settings.temp_offset;
  json["dry"] = Settings.dry;
  json["wet"] = Settings.wet;
  
  
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
