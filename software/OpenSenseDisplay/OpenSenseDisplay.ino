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

// SPI pin definition
#define CS_PIN 15
#define RST_PIN 5
#define DC_PIN 4
#define BUSY_PIN 16

const char *ssid = "blacky_nomap";
const char *password = "SaltAndPepper101";
//const char* ssid     = "vogtland.freifunk.net";
//const char* password = "";

const char *box_id = "5b28d3891fef04001bd7dfae";

String esp_chipid;

const char *host = "api.opensensemap.org";
const int httpsPort = 443;

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

DynamicJsonBuffer jsonBuffer(2500);

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

struct Record
{
  const char *updatedAt;
  const char *temp_sensor_id;
  const char *temp_value;
  const char *temp_unit;
};

Record record;

void loop()
{
  fetchSensorInfo();

  Serial.println(F("done"));

  fetch24hTempHistory();

  Serial.println(F("done"));

  delete(jsonBuffer);

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

  struct tm tm;

  strptime(record.updatedAt, "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

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
  u8g2Fonts.print(String(record.temp_value));
  u8g2Fonts.print(String(record.temp_unit));
}

/*
void drawTemperatureChart()
{
  u8g2Fonts.setFont(u8g2_font_inb27_mf);

  u8g2Fonts.setCursor(50, 300);
  u8g2Fonts.print(String(temp_value));
  u8g2Fonts.print(String(temp_unit));
}
*/

void fetch24hTempHistory()
{
  JsonObject &root = fetch24hTempHistoryAsObject();

  Serial.println(root.size());

  jsonBuffer.clear();

  /*
  float minVal = 0.0;
  float maxVal = 0.0;
  
  float data[24];
  float sumPerHour[24] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  int measurementsPerHour[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  
  for (int i = 0; i < root.size(); i++) {
    JsonObject& obj = root[i];
    
    const char* obj_value = obj["value"]; // "18.40"
    
    JsonArray& obj_location = obj["location"];
    float obj_location0 = obj_location[0]; // 12.138863
    float obj_location1 = obj_location[1]; // 50.497396
    int obj_location2 = obj_location[2]; // 367
    
    const char* obj_createdAt = obj["createdAt"]; // "2018-10-09T11:05:46.821Z"
    
    float val = atof(obj_value);
  
    if (val <= minVal) minVal = val;
    if (val >= maxVal) maxVal = val;
    
    / *
    strptime(obj_createdAt, "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

    sumPerHour[tm.tm_hour] += val;
    measurementsPerHour[tm.tm_hour]++;
    
    * /
  }
  
  for (int i = 0; i < 24; i++) {
    data[i] = sumPerHour[i] / measurementsPerHour[i];
  }
  */
}

void fetchSensorInfo()
{
  JsonObject &root = fetchSensorInfoAsObject();

  yield();

  Serial.println(F("parsing done"));

  Serial.println(F("prepare JSON object"));

  const char *_id = root["_id"];

  JsonArray &sensors = root["sensors"];

  for (int i = 0; i < sensors.size(); i++)
  {
    JsonObject &sensor = sensors[i];

    const char *sensor_title = sensor["title"];
    const char *sensor_unit = sensor["unit"];
    const char *sensor_sensorType = sensor["sensorType"];
    const char *sensor_icon = sensor["icon"];
    const char *sensor_id = sensor["_id"];

    const char *sensor_lastMeasurement_value = sensor["lastMeasurement"]["value"];
    const char *sensor_lastMeasurement_createdAt = sensor["lastMeasurement"]["createdAt"];

    if (strcmp(sensor["title"], "PM10") == 0)
    {
    }
    else if (strcmp(sensor["title"], "PM2.5") == 0)
    {
    }
    else if (strcmp(sensor["title"], "Temperatur") == 0)
    {

      record.temp_sensor_id = sensor_id;
      record.updatedAt = sensor_lastMeasurement_createdAt;
      record.temp_value = sensor_lastMeasurement_value;
      record.temp_unit = sensor_unit;
    }
    else if (strcmp(sensor["title"], "rel. Luftfeuchte") == 0)
    {
    }
    else if (strcmp(sensor["title"], "Luftdruck") == 0)
    {
    }
  }

  jsonBuffer.clear();
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

  String url = "/boxes/" + String(box_id) + "/sensors";

  fetchUrl(client, url);

  Serial.println(F("parsing response"));

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

JsonObject &fetch24hTempHistoryAsObject()
{
  // Connect to HTTPS server
  WiFiClientSecure client;

  struct tm tm;

  strptime(record.updatedAt, "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

  tm.tm_min -= 30;
  mktime(&tm); // normalize it

  char fromDateTime[25];

  strftime(fromDateTime, 25, "%Y-%m-%dT%H:%M:%S.000Z", &tm);

  String url = "/boxes/" + String(box_id) + "/data/" + String(record.temp_sensor_id) + "?from-date=" + String(fromDateTime) + "&to-date=" + String(record.updatedAt);

  fetchUrl(client, url);

  Serial.println(F("parsing response"));

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

String IPAddress2String(const IPAddress &ipaddress)
{
  char myIpString[24];
  sprintf(myIpString, "%d.%d.%d.%d", ipaddress[0], ipaddress[1], ipaddress[2], ipaddress[3]);
  return String(myIpString);
}

void fetchUrl(WiFiClientSecure &client, String url)
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
