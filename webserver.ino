ESP8266WebServer server(80);

const String HTML_BEGIN =
  "<!DOCTYPE html>"
  "<html>"
  "<head>"
  "<title>{title}</title>"
  "<meta charset=\"utf-8\"/>"
  "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\"/>"
  "<style>"
  "div,fieldset,input,select{padding:5px;font-size:1em;}"
  "#container{display:inline-block;min-width:340px;}"
  "body{text-align:center;font-family:verdana,sans-serif;}"
  "div{margin:10px 0; padding:0;}"
  "form{margin:0px;padding:0;}"
  "h1{font-size:1.17em;font-weight:bold;text-align:center;}"
  "h2{font-size:1.5em;font-weight:bold;text-align:center;}"
  "table{text-align:left;}form{margin:5px 0;}"
  "fieldset{border-radius:0.3rem;margin:10px 0;padding:10px;background:#eeeeee;}"
  "legend{font-weight:bold;}"
  "label{display:block;margin:5px 0;text-align:left;cursor:pointer;}"
  "input{padding:10px;border-radius:0.3rem;border:1px solid #cecece;min-width:100%;box-sizing:border-box;}"
  "button{border:0;border-radius:0.3rem;background:#1fa3ec;color:#ffffff;line-height:2.4rem;font-size:1.2rem;width:100%;transition-duration:0.4s;cursor:pointer;}"
  "button:hover{background:#0e70a4;}"
  "button.red{background:#d43535;}button.red:hover{background:#931f1f;}"
  "</style>"
  "</head>"
  "<body>";

const String HTML_END =
  "</body>"
  "</html>";

void webserver_setup() {
  server.on("/", handleRoot);
  server.on("/cn", handleConfigurationForm);
  server.on("/rs", handleRestart);

  server.begin();
}

void webserver_loop() {
  server.handleClient();
}

void handleRoot() {
  String response;
  if (server.hasArg("s")) {
    response = stateTable();
  } else {
    response += HTML_BEGIN;
    response += "<div id=\"container\">";
    response += "<h1>Soil Moisture Sensor</h1>";
    response += "<h2>" + String(Settings.host) + "</h2>";
    response += "<div id=\"s1\">";
    response += stateTable();
    response += "</div><div>";
    response += "<form action=\"/cn\" method=\"GET\">";
    response += "<button>Configuration</button>";
    response += "</form>";
    response += "<form action=\"/rs\" method=\"GET\" onsubmit=\"return confirm('Confirm restart?');\">";
    response += "<button class=\"red\">Restart</button>";
    response += "</form>";
    response += "</div></div>";
    response += "<script>";
    response += "var x=null;function eb(s){return document.getElementById(s);}";
    response += "function la(){if(x!=null){x.abort();}";
    response += "x=new XMLHttpRequest();x.onreadystatechange=function(){if(x.readyState==4&&x.status==200)";
    response += "{var s=x.responseText;eb('s1').innerHTML=s;}};x.open('GET','.?s=1',true);x.send();}";
    response += "window.onload=window.setInterval(la,2345);";
    response += "</script>";
    response += HTML_END;
  }

  response.replace("{title}", Settings.host);

  server.send(200, "text/html", response);
}

String stateTable() {
  String response = "<table style=\"width:100%;\"><tbody>";
  response += "<tr><th>Soil Moisture</th><td>" + String(soil_percent) + " %</td></tr>";

  if (bme_presence) {
    response += "<tr><th>Temperature</th><td>" + String(temperature) + " °C</td></tr>";
    response += "<tr><th>Humidity</th><td>" + String(humidity) + " %</td></tr>";
    response += "<tr><th>Pressure</th><td>" + String(pressure) + " mbar</td></tr>";
  }

  response += "</tbody></table>";

  return response;
}

void handleConfigurationForm() {
  if (handleConfigurationSave()) {
    return;
  }

  String response = HTML_BEGIN;
  response += "<div id=\"container\">";
  response += "<h1>Soil Moisture Sensor</h1><div>";
  response += "<form action=\"/cn\" method=\"POST\" onsubmit=\"return confirm('Are your sure?');\">";
  
  response += "<fieldset><legend>Generic</legend>";
  response += "<label for=\"host\">Hostname</label>";
  response += "<input type=\"text\" name=\"host\" id=\"host\" value=\"" + String(Settings.host) + "\" maxlength=\"40\"/>";
  response += "<label for=\"timezone\">Timezone (h)</label>";
  response += "<input type=\"number\" name=\"timezone\" id=\"timezone\" value=\"" + String(Settings.timezone) + "\" min=\"-12\" max=\"12\"/>";
  response += "</fieldset>";
  
  response += "<fieldset><legend>Calibration</legend>";
  if (bme_presence) {
    response += "<label for=\"temp_offset\">Temperature Offset (°C)</label>";
    response += "<input type=\"number\" name=\"temp_offset\" id=\"temp_offset\" value=\"" + String(Settings.temp_offset) + "\" min=\"-10\" max=\"10\" step=\"0.5\"/>";
  }
  response += "<label for=\"dry\">Dry (mV)</label>";
  response += "<input type=\"number\" name=\"dry\" id=\"dry\" value=\"" + String(Settings.dry) + "\" min=\"550\" max=\"610\"/>";
  response += "<label for=\"wet\">Wet (mV)</label>";
  response += "<input type=\"number\" name=\"wet\" id=\"wet\" value=\"" + String(Settings.wet) + "\" min=\"200\" max=\"300\"/>";
  response += "</fieldset>";

  response += "<fieldset><legend>MQTT</legend>";
  response += "<label for=\"mqtt_host\">Host</label>";
  response += "<input type=\"text\" name=\"mqtt_host\" id=\"mqtt_host\" value=\"" + String(Settings.mqtt_host) + "\" maxlength=\"33\"/>";
  response += "<label for=\"mqtt_port\">Port (1024-49151)</label>";
  response += "<input type=\"number\" name=\"mqtt_port\" id=\"mqtt_port\" value=\"" + String(Settings.mqtt_port) + "\" min=\"1024\" max=\"49151\"/>";
  response += "<label for=\"mqtt_client\">Client-ID</label>";
  response += "<input type=\"text\" name=\"mqtt_client\" id=\"mqtt_client\" value=\"" + String(Settings.mqtt_client) + "\" maxlength=\"33\"/>";
  response += "<label for=\"mqtt_user\">User</label>";
  response += "<input type=\"text\" name=\"mqtt_user\" id=\"mqtt_user\" value=\"" + String(Settings.mqtt_user) + "\" maxlength=\"33\"/>";
  response += "<label for=\"mqtt_pwd\">Password</label>";
  response += "<input type=\"password\" name=\"mqtt_pwd\" id=\"mqtt_pwd\" maxlength=\"33\"/>";
  response += "<label for=\"mqtt_topic\">Topic</label>";
  response += "<input type=\"text\" name=\"mqtt_topic\" id=\"mqtt_topic\" value=\"" + String(Settings.mqtt_topic) + "\" maxlength=\"33\"/>";
  response += "</fieldset>";
  
  response += "<input type=\"hidden\" name=\"do\" value=\"save\"/>";
  response += "<button>Save configuration </button> ";
  response += "</form> ";
  response += "<div>";
  
  response += "<form action=\"/\" method=\"GET\">";
  response += "<button>Main menu</button>";
  response += "</form>";
  response += "</div></div>";
  response += HTML_END;

  String title = String(Settings.host) + " - Configuration";

  response.replace("{title}", title);

  server.send(200, "text/html", response);
}

bool handleConfigurationSave() {
  if (!server.hasArg("do")) {
    return false;
  }

  getStringArg("host", Settings.host, 40);
  Settings.timezone = getIntegerArg("timezone");
  
  Settings.temp_offset = getFloatArg("temp_offset");
  Settings.dry = getIntegerArg("dry");
  Settings.wet = getIntegerArg("wet");

  
  getStringArg("mqtt_host", Settings.mqtt_host, 33);
  Settings.mqtt_port = getIntegerArg("mqtt_port");
  getStringArg("mqtt_client", Settings.mqtt_client, 33);
  getStringArg("mqtt_user", Settings.mqtt_user, 33);

  char temp_out[33];
  getStringArg("mqtt_pwd", temp_out, 33);
  if (strcmp(temp_out, "") > 0) {
    strcpy(Settings.mqtt_pwd, temp_out);  
  }
  
  getStringArg("mqtt_topic", Settings.mqtt_topic, 33);

  saveSettings();
  handleRestart();

  return true;
}

void getStringArg(const char* arg, char* out, size_t max) {
  String s = server.arg(arg);
  strlcpy(out, s.c_str(), max);
}

int getIntegerArg(const char* arg) {
  String s = server.arg(arg);
  return s.toInt();  
}

float getFloatArg(const char* arg) {
  String s = server.arg(arg);
  s.replace(",", ".");
  return s.toFloat();
}

void handleRestart() {
  String response = HTML_BEGIN;
  response += "<div style=\"text-align:center;\"><h1>Trying to reconnect</h1></div>";
  response += "<script>window.setTimeout(function(){location.href = '/';}, 15000);</script>";
  response += HTML_END;

  response.replace("{title}", "Restarting");

  server.send(200, "text/html", response);
  delay(1000);
  Settings.restart_flag = 1;
}
