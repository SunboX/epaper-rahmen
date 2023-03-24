#pragma once

#include <time.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>
#include "Record.h"
#include "Request.h"

void fetch24hTempHistory(String host, String box_id, Record &record);
