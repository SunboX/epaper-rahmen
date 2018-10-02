#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include "Dialog_plain_7.h"
//#include "Chart.h"
#include <ArduinoJson.h>
#include <time.h>
//#include <EEPROM.h>

#define EPD_CS SS

// can use only half buffer size
GxEPD2_BW < GxEPD2_750, GxEPD2_750::HEIGHT / 2 > display(GxEPD2_750(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=5*/ 5, /*BUSY=16*/ 16));

struct tm tm;

// SPI pin definition
#define CS_PIN           15
#define RST_PIN          5
#define DC_PIN           4
#define BUSY_PIN         16

const char* ssid     = "blacky_nomap";
const char* password = "SaltAndPepper101";
//const char* ssid     = "vogtland.freifunk.net";
//const char* password = "";

String esp_chipid;

const char* host = "api.opensensemap.org";
const int httpsPort = 443;

void setup() {

  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while (!Serial) { }

  // set the watch dog timer to 4min, because the display needs some time to update
  ESP.wdtEnable(240000);

#if defined(ESP8266)
  esp_chipid = String(ESP.getChipId());
#endif



  //  debug_out(F("\nChipId: "), DEBUG_MIN_INFO, 0);
  //  debug_out(esp_chipid, DEBUG_MIN_INFO, 1);

  //  display_debug(F("Connecting to"), String(wlanssid));
  connectWifi();            // Start ConnectWifi



}

const char* updatedAt;
const char* sensors2_lastMeasurement_value;
const char* sensors2_unit;

void loop() {

  fetchSensorInfo();

  Serial.println(F("done"));

  Serial.println(F("drawIntoScreen..."));

  drawIntoScreen();

  Serial.println(F("done"));     

  Serial.println(F("Going to sleep now"));

  
  //WiFi.forceSleepBegin(); // power down WiFi
  //delay(2000); // wait some time to have EPD settled

  // Turn the WiFi off before we sleep
  //WiFi.mode(WIFI_OFF);
  //WiFi.forceSleepBegin();

  wdt_reset(); // nodemcu is alive
  yield();
  delay(1000 * 60 * 5);
}


/*****************************************************************
  /* Debug output                                                  *
  /*****************************************************************/
void debug_out(const String& text, const int level, const bool linebreak) {
  //  if (level <= debug) {
  //    if (linebreak) {
  //      Serial.println(text);
  //    } else {
  //      Serial.print(text);
  //    }
  //  }
}

void drawIntoScreen() {
  /*
    const size_t chartBufferSize = 2 * JSON_ARRAY_SIZE(12) + JSON_OBJECT_SIZE(2) + 120;
    DynamicJsonBuffer chartJsonBuffer(chartBufferSize);

    const char* chartJson = "{\"dataset\":[122,65,80,84],\"labels\":[\"Jan\",\"Feb\",\"Mar\",\"Apr\"]}";

    JsonObject& chartData = chartJsonBuffer.parseObject(chartJson);

    JsonArray& dataset = chartData["dataset"];
    JsonArray& labels = chartData["labels"];

    uint16_t xPadding = 30;
    uint16_t yPadding = 30;

    uint16_t height = 200;
    uint16_t width = 200;
  */

  /*
    chart.draw(
       xPadding,
       yPadding,
       height,
       width,
       dataset,
       labels)
    ;
  */

  wdt_reset(); // nodemcu is alive
  yield();

  Serial.println(F("display init"));
  
  display.init(115200);

  wdt_reset(); // nodemcu is alive
  yield();

  display.setRotation(0);
  display.setFont(&Dialog_plain_7);
  display.setTextColor(GxEPD_BLACK);
  
  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 2;
  
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    display.drawLine(20, 200, 619, 200, GxEPD_BLACK);
    display.drawLine(249, 20, 249, 160, GxEPD_BLACK);
    display.drawLine(459, 20, 459, 160, GxEPD_BLACK);
  
  
    strptime(updatedAt, "%Y-%m-%dT%H:%M:%S.%fZ", &tm);
  
    char dateTime[20];
  
    strftime(dateTime, 20, "%d.%m.%Y | %H:%m", &tm);
  
    display.setCursor(10, 10);
    display.print("zuletzt aktualisiert: " + String(dateTime));
  
    display.setFont(&FreeMonoBold24pt7b);
  
    display.setCursor(100, 100);
    display.print(String(sensors2_lastMeasurement_value));
    display.print(" ");
    display.print(String(sensors2_unit));
  }
  while (display.nextPage());
}

void fetchSensorInfo() {

  JsonObject& root = fetchSensorInfoAsObject();

  wdt_reset(); // nodemcu is alive
  yield();

  Serial.println(F("parsing done"));

  Serial.println(F("prepare JSON object"));

  const char* _id = root["_id"]; // "5b28d3891fef04001bd7dfae"
  const char* createdAt = root["createdAt"]; // "2018-06-19T09:57:29.237Z"
  updatedAt = root["updatedAt"]; // "2018-06-24T20:20:00.848Z"
  const char* name = root["name"]; // "PL-Raedel7-airRohr"

  JsonObject& currentLocation = root["currentLocation"];
  const char* currentLocation_timestamp = currentLocation["timestamp"]; // "2018-06-19T09:57:29.232Z"

  JsonArray& currentLocation_coordinates = currentLocation["coordinates"];
  float currentLocation_coordinates0 = currentLocation_coordinates[0]; // 12.138863
  float currentLocation_coordinates1 = currentLocation_coordinates[1]; // 50.497396
  int currentLocation_coordinates2 = currentLocation_coordinates[2]; // 367

  const char* currentLocation_type = currentLocation["type"]; // "Point"

  const char* grouptag = root["grouptag"]; // "PL"
  const char* exposure = root["exposure"]; // "outdoor"

  JsonArray& sensors = root["sensors"];

  JsonObject& sensors0 = sensors[0];
  const char* sensors0_title = sensors0["title"]; // "PM10"
  const char* sensors0_unit = sensors0["unit"]; // "µg/m³"
  const char* sensors0_sensorType = sensors0["sensorType"]; // "SDS 011"
  const char* sensors0_icon = sensors0["icon"]; // "osem-cloud"
  const char* sensors0__id = sensors0["_id"]; // "5b28d3891fef04001bd7dfb2"

  const char* sensors0_lastMeasurement_value = sensors0["lastMeasurement"]["value"]; // "7.40"
  const char* sensors0_lastMeasurement_createdAt = sensors0["lastMeasurement"]["createdAt"]; // "2018-06-24T20:20:00.843Z"

  JsonObject& sensors1 = sensors[1];
  const char* sensors1_title = sensors1["title"]; // "PM2.5"
  const char* sensors1_unit = sensors1["unit"]; // "µg/m³"
  const char* sensors1_sensorType = sensors1["sensorType"]; // "SDS 011"
  const char* sensors1_icon = sensors1["icon"]; // "osem-cloud"
  const char* sensors1__id = sensors1["_id"]; // "5b28d3891fef04001bd7dfb1"

  const char* sensors1_lastMeasurement_value = sensors1["lastMeasurement"]["value"]; // "5.50"
  const char* sensors1_lastMeasurement_createdAt = sensors1["lastMeasurement"]["createdAt"]; // "2018-06-24T20:20:00.843Z"

  JsonObject& sensors2 = sensors[2];
  const char* sensors2_title = sensors2["title"]; // "Temperatur"
  sensors2_unit = sensors2["unit"]; // "°C"
  const char* sensors2_sensorType = sensors2["sensorType"]; // "DHT22"
  const char* sensors2_icon = sensors2["icon"]; // "osem-thermometer"
  const char* sensors2__id = sensors2["_id"]; // "5b28d3891fef04001bd7dfb0"

  sensors2_lastMeasurement_value = sensors2["lastMeasurement"]["value"]; // "13.90"
  const char* sensors2_lastMeasurement_createdAt = sensors2["lastMeasurement"]["createdAt"]; // "2018-06-24T20:20:00.843Z"

  JsonObject& sensors3 = sensors[3];
  const char* sensors3_title = sensors3["title"]; // "rel. Luftfeuchte"
  const char* sensors3_unit = sensors3["unit"]; // "%"
  const char* sensors3_sensorType = sensors3["sensorType"]; // "DHT22"
  const char* sensors3_icon = sensors3["icon"]; // "osem-humidity"
  const char* sensors3__id = sensors3["_id"]; // "5b28d3891fef04001bd7dfaf"

  const char* sensors3_lastMeasurement_value = sensors3["lastMeasurement"]["value"]; // "83.30"
  const char* sensors3_lastMeasurement_createdAt = sensors3["lastMeasurement"]["createdAt"]; // "2018-06-24T20:20:00.843Z"

  const char* model = root["model"]; // "luftdaten_sds011_dht22"
  const char* description = root["description"]; // "luftdaten.info airRohr (#13905634) placed at street side in the city center of Plauen\n\nusing:\n- SDS011\n- DHT22\n- BMP180"
  const char* image = root["image"]; // "5b28d3891fef04001bd7dfae_pao35m.jpg"

  JsonObject& loc0_geometry = root["loc"][0]["geometry"];
  const char* loc0_geometry_timestamp = loc0_geometry["timestamp"]; // "2018-06-19T09:57:29.232Z"

  JsonArray& loc0_geometry_coordinates = loc0_geometry["coordinates"];
  float loc0_geometry_coordinates0 = loc0_geometry_coordinates[0]; // 12.138863
  float loc0_geometry_coordinates1 = loc0_geometry_coordinates[1]; // 50.497396
  int loc0_geometry_coordinates2 = loc0_geometry_coordinates[2]; // 367

  const char* loc0_geometry_type = loc0_geometry["type"]; // "Point"

  const char* loc0_type = root["loc"][0]["type"]; // "Feature"
}

void connectWifi() {
  
  #ifdef RE_INIT_NEEDED
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA); // switch off AP
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    WiFi.disconnect();
  #endif

  if (!WiFi.getAutoConnect() || ( WiFi.getMode() != WIFI_STA) || (WiFi.SSID() != ssid))
  {
    Serial.println();
    Serial.print("WiFi.getAutoConnect()=");
    Serial.println(WiFi.getAutoConnect());
    Serial.print("WiFi.SSID()=");
    Serial.println(WiFi.SSID());
    WiFi.mode(WIFI_STA); // switch off AP
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
  }
  int ConnectTimeout = 30; // seconds
  
  //int retry_count = 0;
  //while ((WiFi.status() != WL_CONNECTED) && (retry_count < 40)) {
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    Serial.print(WiFi.status());
    if (--ConnectTimeout <= 0)
    {
      Serial.println();
      Serial.println("WiFi connect timeout");
      return;
    }
  }
  Serial.println();
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

JsonObject& fetchSensorInfoAsObject() {

  // Connect to HTTPS server
  WiFiClientSecure client;
  Serial.print(F("connecting to "));
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println(F("connection failed"));
  }

  wdt_reset(); // nodemcu is alive
  yield();

  String url = F("/boxes/5b28d3891fef04001bd7dfae");
  Serial.print(F("requesting URL: "));
  Serial.println(url);

  wdt_reset(); // nodemcu is alive
  yield();

  // Make a HTTP request:
  client.print(F("GET "));
  client.print(url);
  client.print(F(" HTTP/1.0"));
  client.println();

  client.print(F("Host: "));
  client.print(host);
  client.println();

  client.println(F("User-Agent: OpenSenseDisplay"));
  client.println(F("Connection: close"));
  if (client.println() == 0) {
    Serial.println(F("Failed to send request"));
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
  }

  wdt_reset(); // nodemcu is alive
  yield();

  /*
    String payload;

    if (httpCode == HTTP_CODE_OK) {
    payload = client.getString();
    } else {
    Serial.println(httpCode);
    Serial.printf("[HTTP] GET... failed, error: %s\n", client.errorToString(httpCode).c_str());

    //    Serial.println("Going to sleep now");
    //    esp_deep_sleep_start();
    }

    client.end();
  */

  Serial.print(F("free heap: "));
  Serial.println(ESP.getFreeHeap());

  Serial.println(F("parsing response"));

  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t bufferSize = JSON_ARRAY_SIZE(1) + 2 * JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 6 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + 5 * JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 1490;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  //StaticJsonBuffer<2585> jsonBuffer;

  wdt_reset(); // nodemcu is alive
  yield();

  Serial.print(F("free heap: "));
  Serial.println(ESP.getFreeHeap());

  JsonObject& root = jsonBuffer.parseObject(client);

  wdt_reset(); // nodemcu is alive
  yield();

  client.stop();

  wdt_reset(); // nodemcu is alive
  yield();

  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
  }

  return root;
}

String IPAddress2String(const IPAddress& ipaddress) {
  char myIpString[24];
  sprintf(myIpString, "%d.%d.%d.%d", ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]);
  return String(myIpString);
}

