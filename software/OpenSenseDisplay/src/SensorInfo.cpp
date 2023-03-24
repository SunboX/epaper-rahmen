#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include "SensorInfo.h"

DynamicJsonDocument fetchSensorInfoAsObject(String box_id)
{
  // Connect to HTTPS server
  WiFiClientSecure client;

  String url = "https://api.opensensemap.org/boxes/" + box_id + "/sensors";

  fetchUrl(client, url);

  Serial.println(F("parsing response"));

  Serial.print(F("free heap: "));
  
  Serial.println(ESP.getFreeHeap());const size_t capacity = JSON_ARRAY_SIZE(5) + 6*JSON_OBJECT_SIZE(2) + 5*JSON_OBJECT_SIZE(6) + 3000;
  DynamicJsonDocument doc(capacity);
  
  auto error = deserializeJson(doc, client);

  client.stop();

  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
  }

  return doc;
}

void fetchSensorInfo(String host, String box_id, Record &record)
{
  DynamicJsonDocument doc = fetchSensorInfoAsObject(box_id);

  Serial.println(F("parsing done"));

  Serial.println(F("prepare JSON object"));
  
  const char* _id = doc["_id"];
  
  JsonArray sensors = doc["sensors"];

  for (int i = 0; i < sensors.size(); i++)
  
  {
    JsonObject sensor = sensors[i];

    const char *sensor_title = sensor["title"];
    const char *sensor_unit = sensor["unit"];
    const char *sensor_sensorType = sensor["sensorType"];
    const char *sensor_icon = sensor["icon"];
    const char *sensor_id = sensor["_id"];

    const char *sensor_lastMeasurement_value = sensor["lastMeasurement"]["value"];
    const char *sensor_lastMeasurement_createdAt = sensor["lastMeasurement"]["createdAt"];

    if (strcmp(sensor["title"], "PM10") == 0)
    {
      record.pm10_sensor_id = sensor_id;
      record.pm10_value = sensor_lastMeasurement_value;
      record.pm10_unit = sensor_unit;
    }
    else if (strcmp(sensor["title"], "PM2.5") == 0)
    {
      record.pm25_sensor_id = sensor_id;
      record.pm25_value = sensor_lastMeasurement_value;
      record.pm25_unit = sensor_unit;
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

  //free(doc);
}
