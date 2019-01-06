#include "Request.h"

void fetchUrl(WiFiClientSecure &client, String url)
{
  UrlParserResult result;
  UrlParser::parse(url.c_str(), result);
  
  Serial.print(F("connecting to "));
  Serial.println(result.host);
  if (!client.connect(result.host, httpsPort))
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
  client.print(result.host);
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
