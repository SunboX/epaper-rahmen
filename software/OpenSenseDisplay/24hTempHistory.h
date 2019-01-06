#pragma once

#include <time.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "Record.h"
#include "Request.h"

void fetch24hTempHistory(String host, String box_id, Record &record);
