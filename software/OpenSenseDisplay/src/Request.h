#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include "UrlParser.h"

const int httpsPort = 443;

void fetchUrl(WiFiClientSecure &client, String url);
