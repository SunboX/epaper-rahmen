
#include "DEV_Config.h"
#include "Record.h"
#include "WiFi.h"
#include "SensorInfo.h"
#include "InternalSensor.h"
#include "24hTempHistory.h"
#include "Display.h"

const char* ssid     = "vogtland.freifunk.net";
const char* password = "";

const char *box_id = "5b28d3891fef04001bd7dfae";

String esp_chipid;

const char *host = "api.opensensemap.org";

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while (!Serial)
  {
  }

  //  debug_out(F("\nChipId: "), DEBUG_MIN_INFO, 0);
  //  debug_out(esp_chipid, DEBUG_MIN_INFO, 1);

  initializeInternalSensor();

  //  display_debug(F("Connecting to"), String(wlanssid));
  connectWifi(ssid, password); // Start ConnectWifi
}

void loop()
{
  Record record;

  readInternalSensor(record);

  Serial.println(F("done"));

  fetchSensorInfo(host, box_id, record);

  Serial.println(F("done"));

  fetch24hTempHistory(host, box_id, record);

  Serial.println(F("done"));

  Serial.println(F("drawIntoScreen..."));

  drawIntoScreen(record);

  Serial.println(F("done"));

  Serial.println(F("Going to sleep now"));

  //WiFi.forceSleepBegin(); // power down WiFi
  //delay(2000); // wait some time to have EPD settled

  // Turn the WiFi off before we sleep
  //WiFi.mode(WIFI_OFF);
  //WiFi.forceSleepBegin();

  delay(1000 * 60 * 5);
}
