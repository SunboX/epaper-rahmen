#include <GxEPD.h>

// small
//#include <GxGDEP015OC1/GxGDEP015OC1.cpp>

// big 7.5
#include <GxGDEW075T8/GxGDEW075T8.cpp>

#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include "pf_tempesta_seven5pt7b.h"
#include "Chart.h"
#include <WiFi.h>

// ESP32
#include <HTTPClient.h>

// ESP8266
//#include <WifiClient.h>

#include <ArduinoJson.h>
#include <Time.h>
#include <Fonts/FreeMonoBold24pt7b.h>

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5 * 60        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// Network information
const char* ssid = "vogtland.freifunk.net";
const char* password = "";

// ESP32
GxIO_Class io(SPI, SS, 22, 21);
GxEPD_Class display(io, 16, 4);

// ESP8266
//GxIO_Class io(SPI, /*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
//GxEPD_Class display(io /*RST=D4*/ /*BUSY=D2*/); // default selection of D4(=2), D2(=4)

Chart chart(display);

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

void setup() {

  Serial.begin(115200);
  delay(10);

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  /*
    First we configure the wake up source
    We set our ESP32 to wake up every 60 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
                 " Seconds");

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  display.init();

  const GFXfont* f = &pf_tempesta_seven5pt7b;

  //display.setRotation(45);
  display.setFont(f);
  display.setTextColor(GxEPD_BLACK);
  display.fillRect(0, 0, 200, 200, GxEPD_WHITE);
  display.setCursor(0, 7);

  //  String out = "";

  delay(3000);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //
  //    // get current date/time
  //    struct tm timeinfo;
  //    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  //
  //    if (!getLocalTime(&timeinfo))
  //    {
  //      Serial.println(F(" Failed to obtain time"));
  //
  //      Serial.println("Going to sleep now");
  //      esp_deep_sleep_start();
  //    }
  //
  //    char s[29];
  //    strftime(s, sizeof(s), "Date/Time: %d-%m-%y %H:%M:%S", &timeinfo);
  //    out += String(s) + "\n";
  //
  //    Serial.println(&timeinfo, " %d %B %Y %H:%M:%S ");
  //    Serial.println("");
  //
  //    // print your WiFi shield's IP address:
  //    IPAddress ip = WiFi.localIP();
  //    Serial.print("IP Address: ");
  //    Serial.println(ip);
  //
  //    out += "IP Address: " + ip.toString() + "\n";
  //
  //    // print the received signal strength:
  //    long rssi = WiFi.RSSI();
  //    Serial.print("signal strength (RSSI):");
  //    Serial.print(rssi);
  //    Serial.println(" dBm");
  //
  //    out += "Signal Strength: " + String(rssi) + " dBm\n";
  //
  //    Serial.println(F("Connecting..."));

  // Connect to HTTP server
  //HTTPClient client;

  //client.begin("http://nextnode.ffv/cgi-bin/nodeinfo");

  //int httpCode = client.GET();

  //  String payload;
  //
  //  if (httpCode == HTTP_CODE_OK) {
  //    payload = client.getString();
  //  } else {
  //    Serial.printf("[HTTP] GET... failed, error: %s\n", client.errorToString(httpCode).c_str());
  //
  //    Serial.println("Going to sleep now");
  //    esp_deep_sleep_start();
  //  }
  //
  //  client.end();
  //
  //  // Allocate JsonBuffer
  //  // Use arduinojson.org/assistant to compute the capacity.
  //  const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 5 * JSON_OBJECT_SIZE(1) + 6 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(8) + 1024;
  //  DynamicJsonBuffer jsonBuffer(bufferSize);
  //
  //  JsonObject& root = jsonBuffer.parseObject(payload);
  //
  //  if (!root.success()) {
  //    Serial.println(F("Parsing failed!"));
  //
  //    Serial.println("Going to sleep now");
  //    esp_deep_sleep_start();
  //  }
  //
  //  // Extract values
  //
  //  JsonObject& software = root["software"];
  //
  //  const char* software_autoupdater_branch = software["autoupdater"]["branch"]; // "stable"
  //  bool software_autoupdater_enabled = software["autoupdater"]["enabled"]; // true
  //
  //  out += "Node ID: " + String(node_id) + "\n";
  //  out += "Name: " + String(hostname) + "\n";
  //  out += "Firmware: " + String(software_firmware_release) + "\n";
  //  out += "Hardware: " + String(hardware_model) + "\n";
  //  out += "Clients: " + String(clients_total) + " (2.4: " + String(clients_wifi24) + ", 5: " + String(clients_wifi5) + ")\n";
  //  out += "Traffic RX: " + prettyBytes(traffic_rx_bytes * -1) + "\n";
  //  out += "Traffic TX: " + prettyBytes(traffic_tx_bytes) + "\n";
  //  out += "Geo: " + String(location_latitude, 4) + "  " + String(location_longitude, 4) + "\n";
  //  out += "Owner: " + String(owner_contact) + "\n";
  //
  //  display.print(out);
  //  display.update();

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

  chart.draw(
     xPadding,
     yPadding,
     height,
     width,
     dataset,
     labels)
  ;

  display.update();

  Serial.println("Going to sleep now");

  esp_deep_sleep_start();
}

void loop() {
  //This is not going to be called
}

void printLine(int line, String str) {

}

void showPartialUpdate(float temperature)
{
  /*
    String temperatureString = String(temperature,1);
    const char* name = "FreeSansBold24pt7b";
    const GFXfont* f = &FreeSansBold24pt7b;

    uint16_t 0 = 60;
    uint16_t 0 = 60;
    uint16_t 200 = 90;
    uint16_t 200 = 100;
    uint16_t cursor_y = 0 + 16;

    display.setRotation(45);
    display.setFont(f);
    display.setTextColor(GxEPD_BLACK);

    display.fillRect(0, 0, 200, 200, GxEPD_WHITE);
    display.setCursor(0, cursor_y+38);
    display.print(temperatureString);
    display.updateWindow(0, 0, 200, 200, true);
  */
}

String prettyBytes(long bytes) {
  char buf[128] = { 0 };
  int i = 0;
  const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
  while (bytes > 1024) {
    bytes /= 1024;
    i++;
  }
  sprintf(buf, "%.*f %s", i, (float)bytes, units[i]);
  return (char*)buf;
}


/*
  Method to print the reason by which ESP32
  has been awaken from sleep
*/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); break;
  }
}

