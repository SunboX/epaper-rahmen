
#include <ESP8266WiFi.h>
#include <GxEPD.h>
#include <GxGDEW075T8/GxGDEW075T8.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/FreeMonoBold24pt7b.h>
#include "pf_tempesta_seven5pt7b.h"
//#include "Chart.h"
#include <ArduinoJson.h>

// SPI pin definition 
#define CS_PIN           15
#define RST_PIN          5
#define DC_PIN           4
#define BUSY_PIN         16

GxIO_Class io(SPI, CS_PIN, DC_PIN, RST_PIN);
GxEPD_Class display(io, RST_PIN, BUSY_PIN);

char wlanssid[35] = "vogtland.freifunk.net";
char wlanpwd[65] = "";

String esp_chipid;

void setup() {
  
  #if defined(ESP8266)
    esp_chipid = String(ESP.getChipId());
  #endif

  Serial.begin(115200);
  delay(10);

  display.init();
  
  debug_out(F("\nChipId: "), DEBUG_MIN_INFO, 0);
  debug_out(esp_chipid, DEBUG_MIN_INFO, 1);

  display_debug(F("Connecting to"), String(wlanssid));
  connectWifi();            // Start ConnectWifi

  const GFXfont* f = &FreeMonoBold24pt7b;

  //display.setRotation(45);
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);
  display.fillRect(0, 0, 200, 200, GxEPD_WHITE);

  display.setCursor(10, 100);
  display.print("Hello World!");

  // 640 x 384
  display.drawLine(0, 0, 639, 0, GxEPD_BLACK);
  display.drawLine(639, 0, 639, 383, GxEPD_BLACK);
  display.drawLine(639, 383, 0, 383, GxEPD_BLACK);
  display.drawLine(0, 383, 0, 0, GxEPD_BLACK);

  display.update();

  Serial.println("done");

  
  // Connect to HTTP server
  HTTPClient client;

  client.begin("https://api.opensensemap.org/boxes/5b28d3891fef04001bd7dfae");

  int httpCode = client.GET();

  String payload;

  if (httpCode == HTTP_CODE_OK) {
    payload = client.getString();
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", client.errorToString(httpCode).c_str());

    Serial.println("Going to sleep now");
    esp_deep_sleep_start();
  }

  client.end();

  const size_t bufferSize = JSON_ARRAY_SIZE(1) + 2 * JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 6 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + 5 * JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 1490;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  JsonObject& root = jsonBuffer.parseObject(payload);

  const char* _id = root["_id"]; // "5b28d3891fef04001bd7dfae"
  const char* createdAt = root["createdAt"]; // "2018-06-19T09:57:29.237Z"
  const char* updatedAt = root["updatedAt"]; // "2018-06-24T20:20:00.848Z"
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
  const char* sensors2_unit = sensors2["unit"]; // "°C"
  const char* sensors2_sensorType = sensors2["sensorType"]; // "DHT22"
  const char* sensors2_icon = sensors2["icon"]; // "osem-thermometer"
  const char* sensors2__id = sensors2["_id"]; // "5b28d3891fef04001bd7dfb0"

  const char* sensors2_lastMeasurement_value = sensors2["lastMeasurement"]["value"]; // "13.90"
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

  //display.setCursor(10, 10);
  //display.print("zuletzt aktualisiert: " + String(updatedAt));


  //display.setFont(&FreeMonoBold24pt7b);
  //
  //display.setCursor(10, 10);
  //display.print(sensors2_lastMeasurement_value);
  //display.print(sensors2_unit);



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
  display.update();

  Serial.println("Going to sleep now");

}

void loop() {
  // put your main code here, to run repeatedly:

}


/*****************************************************************
/* Debug output                                                  *
/*****************************************************************/
void debug_out(const String& text, const int level, const bool linebreak) {
  if (level <= debug) {
    if (linebreak) {
      Serial.println(text);
    } else {
      Serial.print(text);
    }
  }
}

/*****************************************************************
/* WiFi auto connecting script                                   *
/*****************************************************************/
void connectWifi() {
  int retry_count = 0;
  
  debug_out(String(WiFi.status()), DEBUG_MIN_INFO, 1);
  
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(wlanssid, wlanpwd); // Start WiFI

  debug_out(F("Connecting to "), DEBUG_MIN_INFO, 0);
  debug_out(wlanssid, DEBUG_MIN_INFO, 1);

  while ((WiFi.status() != WL_CONNECTED) && (retry_count < 40)) {
    delay(500);
    debug_out(".", DEBUG_MIN_INFO, 0);
    retry_count++;
  }
  
  debug_out(F("WiFi connected\nIP address: "), DEBUG_MIN_INFO, 0);
  debug_out(IPAddress2String(WiFi.localIP()), DEBUG_MIN_INFO, 1);
}


/*****************************************************************
/* IPAddress to String                                           *
/*****************************************************************/
String IPAddress2String(const IPAddress& ipaddress) {
  char myIpString[24];
  sprintf(myIpString, "%d.%d.%d.%d", ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]);
  return String(myIpString);
}
