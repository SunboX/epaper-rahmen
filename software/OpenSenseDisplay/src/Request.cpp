#include "Request.h"

void fetchUrl(WiFiClientSecure &client, String url)
{
  Serial.print(F("trying to fetch URL: "));
  Serial.println(url);
  
  UrlParserResult result;
  UrlParser::parse(url.c_str(), result);
  
  Serial.print(F("connecting to "));
  Serial.println(result.host);
  if (!client.connect(result.host.c_str(), httpsPort))
  {
    Serial.println(F("connection failed"));
  }

  Serial.print(F("requesting URL: "));
  Serial.println(result.pathAndQuery());

  yield();

  // Make a HTTP request:
  client.print(F("GET "));
  client.print(result.pathAndQuery());
  client.print(F(" HTTP/1.0"));
  client.println();

  client.print(F("Host: "));
  client.print(result.host);
  client.println();

  client.println(F("User-Agent: OpenSenseDisplay"));
  client.println(F("Connection: close"));
  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    ESP.restart();
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.0 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    ESP.restart();
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    ESP.restart();
  }

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
