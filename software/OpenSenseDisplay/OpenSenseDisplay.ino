#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>
//#include "Chart.h"
#include <ArduinoJson.h>
#include <time.h>
//#include <EEPROM.h>

#define EPD_CS SS

// can use only half buffer size
GxEPD2_BW<GxEPD2_750, GxEPD2_750::HEIGHT / 2> display(GxEPD2_750(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=5*/ 5, /*BUSY=16*/ 16));

struct tm tm;

// SPI pin definition
#define CS_PIN 15
#define RST_PIN 5
#define DC_PIN 4
#define BUSY_PIN 16

const char *ssid = "blacky_nomap";
const char *password = "SaltAndPepper101";
//const char* ssid     = "vogtland.freifunk.net";
//const char* password = "";

String esp_chipid;

const char *host = "api.opensensemap.org";
const int httpsPort = 443;

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while (!Serial)
  {
  }

  // set the watch dog timer to 4min, because the display needs some time to update
  ESP.wdtEnable(240000);

#if defined(ESP8266)
  esp_chipid = String(ESP.getChipId());
#endif

  //  debug_out(F("\nChipId: "), DEBUG_MIN_INFO, 0);
  //  debug_out(esp_chipid, DEBUG_MIN_INFO, 1);

  //  display_debug(F("Connecting to"), String(wlanssid));
  connectWifi(); // Start ConnectWifi
}

/*
struct record_type
{
  const char *updatedAt;
  const char *temp_value;
  const char *temp_unit;
};

typedef struct record_type Record;
Record record;
*/
const char *updatedAt;
const char *temp_value;
const char *temp_unit;

void loop()
{
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

  yield();
  delay(1000 * 60 * 5);
}

/*****************************************************************
  /* Debug output                                                  *
  /*****************************************************************/
void debug_out(const String &text, const int level, const bool linebreak)
{
  //  if (level <= debug) {
  //    if (linebreak) {
  //      Serial.println(text);
  //    } else {
  //      Serial.print(text);
  //    }
  //  }
}

void drawIntoScreen()
{
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

  yield();

  Serial.println(F("display init"));

  display.init(115200);

  yield();

  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX
  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

  yield();

  display.setTextColor(GxEPD_BLACK);

  u8g2Fonts.setFontMode(1);                  // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);             // left to right (this is default)
  u8g2Fonts.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color

  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 2;

  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    drawLines();

    drawUpdatedTime();

    drawTemperatureBox();

  } while (display.nextPage());
}

void drawLines()
{
  display.drawLine(20, 200, 619, 200, GxEPD_BLACK);
  display.drawLine(249, 20, 249, 160, GxEPD_BLACK);
  display.drawLine(459, 20, 459, 160, GxEPD_BLACK);
}

void drawUpdatedTime()
{
  u8g2Fonts.setFont(u8g2_font_4x6_tf);

  strptime(updatedAt, "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

  tm.tm_hour += 2; // add 2 hous to the time
  mktime(&tm);     // normalize it

  char dateTime[20];

  strftime(dateTime, 20, "%d.%m.%Y | %H:%M", &tm);

  u8g2Fonts.setCursor(10, 10);
  u8g2Fonts.print("AKTUALISIERT: " + String(dateTime));
}

void drawTemperatureBox()
{
  u8g2Fonts.setFont(u8g2_font_inb27_mf);

  u8g2Fonts.setCursor(50, 100);
  u8g2Fonts.print(String(temp_value));
  u8g2Fonts.print(String(temp_unit));
}

/*
void drawTemperatureChart()
{
  u8g2Fonts.setFont(u8g2_font_inb27_mf);

  u8g2Fonts.setCursor(50, 300);
  u8g2Fonts.print(String(temp_value));
  u8g2Fonts.print(String(temp_unit));
}

void fetch24hTempHistory()
{
  JsonObject &root = fetch24hTempHistoryAsObject();

  JsonObject &root_0 = root[0];
  const char *root_0_value = root_0["value"]; // "13.00"

  JsonArray &root_0_location = root_0["location"];
  float root_0_location0 = root_0_location[0]; // 12.138863
  float root_0_location1 = root_0_location[1]; // 50.497396
  int root_0_location2 = root_0_location[2];   // 367

  const char *root_0_createdAt = root_0["createdAt"]; // "2018-10-08T20:19:53.631Z"
}
*/

void fetchSensorInfo()
{
  JsonObject &root = fetchSensorInfoAsObject();

  yield();

  Serial.println(F("parsing done"));

  Serial.println(F("prepare JSON object"));

  const char *_id = root["_id"]; // "5b28d3891fef04001bd7dfae"

  JsonArray &sensors = root["sensors"];

  JsonObject &sensors0 = sensors[0];
  const char *sensors0_title = sensors0["title"];           // "PM10"
  const char *sensors0_unit = sensors0["unit"];             // "µg/m³"
  const char *sensors0_sensorType = sensors0["sensorType"]; // "SDS 011"
  const char *sensors0_icon = sensors0["icon"];             // "osem-cloud"
  const char *sensors0__id = sensors0["_id"];               // "5b28d3891fef04001bd7dfb2"

  const char *sensors0_lastMeasurement_value = sensors0["lastMeasurement"]["value"];         // "23.10"
  const char *sensors0_lastMeasurement_createdAt = sensors0["lastMeasurement"]["createdAt"]; // "2018-10-08T20:14:55.228Z"

  JsonObject &sensors1 = sensors[1];
  const char *sensors1_title = sensors1["title"];           // "PM2.5"
  const char *sensors1_unit = sensors1["unit"];             // "µg/m³"
  const char *sensors1_sensorType = sensors1["sensorType"]; // "SDS 011"
  const char *sensors1_icon = sensors1["icon"];             // "osem-cloud"
  const char *sensors1__id = sensors1["_id"];               // "5b28d3891fef04001bd7dfb1"

  const char *sensors1_lastMeasurement_value = sensors1["lastMeasurement"]["value"];         // "18.87"
  const char *sensors1_lastMeasurement_createdAt = sensors1["lastMeasurement"]["createdAt"]; // "2018-10-08T20:14:55.228Z"

  JsonObject &sensors2 = sensors[2];
  const char *sensors2_title = sensors2["title"];           // "Temperatur"
  const char *sensors2_unit = sensors2["unit"];             // "°C"
  const char *sensors2_sensorType = sensors2["sensorType"]; // "DHT22"
  const char *sensors2_icon = sensors2["icon"];             // "osem-thermometer"
  const char *sensors2__id = sensors2["_id"];               // "5b28d3891fef04001bd7dfb0"

  const char *sensors2_lastMeasurement_value = sensors2["lastMeasurement"]["value"];         // "12.90"
  const char *sensors2_lastMeasurement_createdAt = sensors2["lastMeasurement"]["createdAt"]; // "2018-10-08T20:14:55.228Z"

  JsonObject &sensors3 = sensors[3];
  const char *sensors3_title = sensors3["title"];           // "rel. Luftfeuchte"
  const char *sensors3_unit = sensors3["unit"];             // "%"
  const char *sensors3_sensorType = sensors3["sensorType"]; // "DHT22"
  const char *sensors3_icon = sensors3["icon"];             // "osem-humidity"
  const char *sensors3__id = sensors3["_id"];               // "5b28d3891fef04001bd7dfaf"

  const char *sensors3_lastMeasurement_value = sensors3["lastMeasurement"]["value"];         // "99.90"
  const char *sensors3_lastMeasurement_createdAt = sensors3["lastMeasurement"]["createdAt"]; // "2018-10-08T20:14:55.228Z"

  updatedAt = sensors2_lastMeasurement_createdAt;
  temp_value = sensors2_lastMeasurement_value;
  temp_unit = sensors2_unit;
}

void connectWifi()
{
#ifdef RE_INIT_NEEDED
  WiFi.persistent(true);
  WiFi.mode(WIFI_STA); // switch off AP
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.disconnect();
#endif

  if (!WiFi.getAutoConnect() || (WiFi.getMode() != WIFI_STA) || (WiFi.SSID() != ssid))
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

JsonObject &fetchSensorInfoAsObject()
{
  // Connect to HTTPS server
  WiFiClientSecure client;

  String url = F("/boxes/5b28d3891fef04001bd7dfae/sensors");

  //fetchUrl(&client, url);

  Serial.print(F("connecting to "));
  Serial.println(host);
  if (!client.connect(host, httpsPort))
  {
    Serial.println(F("connection failed"));
  }

  yield();

  Serial.print(F("requesting URL: "));
  Serial.println(url);

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
  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
  }

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
  const size_t bufferSize = JSON_ARRAY_SIZE(5) + 6 * JSON_OBJECT_SIZE(2) + 5 * JSON_OBJECT_SIZE(6) + 900;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  yield();

  Serial.print(F("free heap: "));
  Serial.println(ESP.getFreeHeap());

  JsonObject &root = jsonBuffer.parseObject(client);

  yield();

  client.stop();

  yield();

  if (!root.success())
  {
    Serial.println(F("Parsing failed!"));
  }

  return root;
}

/*
JsonObject &fetch24hTempHistoryAsObject()
{
  // Connect to HTTPS server
  WiFiClientSecure client;

  String url = F("boxes/5b28d3891fef04001bd7dfae/data/5b28d3891fef04001bd7dfb0?from-date=2018-10-07T20:22:22.729Z&to-date=2018-10-08T20:22:22.729Z");

  fetchUrl(client, url);

  Serial.println(F("parsing response"));

  // Allocate JsonBuffer
  // Use arduinojson.org/assistant to compute the capacity.
  const size_t bufferSize = 578 * JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(578) + 578 * JSON_OBJECT_SIZE(3) + 51380;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  yield();

  Serial.print(F("free heap: "));
  Serial.println(ESP.getFreeHeap());

  JsonObject &root = jsonBuffer.parseObject(client);

  yield();

  client.stop();

  yield();

  if (!root.success())
  {
    Serial.println(F("Parsing failed!"));
  }

  return root;
}
*/

String IPAddress2String(const IPAddress &ipaddress)
{
  char myIpString[24];
  sprintf(myIpString, "%d.%d.%d.%d", ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]);
  return String(myIpString);
}

void fetchUrl(WiFiClientSecure client, String url)
{
  Serial.print(F("connecting to "));
  Serial.println(host);
  if (!client.connect(host, httpsPort))
  {
    Serial.println(F("connection failed"));
  }

  yield();

  Serial.print(F("requesting URL: "));
  Serial.println(url);

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
  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
  }

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
}
