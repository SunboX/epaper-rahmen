#include "24hTempHistory.h"

JsonObject &fetch24hTempHistoryAsObject(String host, String box_id, Record &record)
{
  // Connect to HTTPS server
  WiFiClientSecure client;

  struct tm tm;

  strptime(record.updatedAt.c_str(), "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

  tm.tm_min -= 30;
  mktime(&tm); // normalize it

  char fromDateTime[25];

  strftime(fromDateTime, 25, "%Y-%m-%dT%H:%M:%S.000Z", &tm);

  String url = "/boxes/" + box_id + "/data/" + String(record.temp_sensor_id) + "?from-date=" + String(fromDateTime) + "&to-date=" + String(record.updatedAt);

  fetchUrl(client, url);

  Serial.println(F("parsing response"));

  yield();

  Serial.print(F("free heap: "));
  Serial.println(ESP.getFreeHeap());

  DynamicJsonBuffer jsonBuffer(2500);

  JsonObject &root = jsonBuffer.parseObject(client);

  jsonBuffer.clear();
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

void fetch24hTempHistory(String host, String box_id, Record &record)
{
  JsonObject &root = fetch24hTempHistoryAsObject(host, box_id, record);

  Serial.println(root.size());

  free(&root);

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
