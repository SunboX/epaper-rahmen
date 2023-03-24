#include "24hTempHistory.h"

DynamicJsonDocument fetch24hTempHistoryAsObject(String host, String box_id, Record &record)
{
  // Connect to HTTPS server
  WiFiClientSecure client;

  struct tm tm;

  strptime(record.updatedAt.c_str(), "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

  tm.tm_hour -= 24;
  mktime(&tm); // normalize it

  char fromDateTime[25];

  strftime(fromDateTime, 25, "%Y-%m-%dT%H:%M:%S.000Z", &tm);

  String url = "https://api.opensensemap.org/statistics/descriptive?boxId=" + box_id + "&from-date=" + String(fromDateTime) + "&to-date=" + String(record.updatedAt) + "&phenomenon=Temperatur&operation=arithmeticMean&window=3600000&format=json";

  fetchUrl(client, url);

  Serial.println(F("parsing response"));

  Serial.print(F("free heap: "));
  Serial.println(ESP.getFreeHeap());

  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(26) + 2000;
  DynamicJsonDocument doc(capacity);

  auto error = deserializeJson(doc, client);

  client.stop();

  if (error) {
    Serial.print(F("deserializeJson() failed with code "));
    Serial.println(error.c_str());
  }

  return doc;
}

void fetch24hTempHistory(String host, String box_id, Record &record)
{
  DynamicJsonDocument doc = fetch24hTempHistoryAsObject(host, box_id, record);

  Serial.println(doc.size());

  float minVal = 0.0;
  float maxVal = 0.0;

  JsonObject root = doc[0];

  int i = 0;
  for (const auto& kv : root) {

    const char* key = kv.key().c_str(); // "2018-10-09T11:05:46.821Z"

    if (strcmp(key, "sensorId") == 0) {
      continue;
    }

    float val = kv.value().as<float>(); // "18.40"

    Serial.println(key);
    Serial.println(val);

    if (val <= minVal) {
      minVal = val;
    }
    if (val >= maxVal) {
      maxVal = val;
    }

    struct tm tm;

    strptime(key, "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

    if (isnan(val)) {
      val = 0;
    }

    Serial.print("hour ");
    Serial.println(tm.tm_hour + 1);

    int h = tm.tm_hour + 1;

    if (h > 999) {
      h = 1;
    }

    record.data_24h_temp_history[i] = val;
    record.data_24h_temp_history_labels[i] = h;

    i++;
  }
}
