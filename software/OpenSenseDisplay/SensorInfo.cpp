#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "SensorInfo.h"

JsonObject &fetchSensorInfoAsObject(String box_id)
{
  // Connect to HTTPS server
  WiFiClientSecure client;

  String url = "/boxes/" + box_id + "/sensors";

  fetchUrl(client, url);

  Serial.println(F("parsing response"));

  yield();

  Serial.print(F("free heap: "));
  Serial.println(ESP.getFreeHeap());

  DynamicJsonBuffer jsonBuffer(2500);

  JsonObject &root = jsonBuffer.parseObject(client);

  free(&jsonBuffer);

  yield();

  client.stop();

  yield();

  if (!root.success())
  {
    Serial.println(F("Parsing failed!"));
  }

  return root;
}

void fetchSensorInfo(String host, String box_id, Record &record)
{
  JsonObject &root = fetchSensorInfoAsObject(box_id);

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

  free(&root);
}
