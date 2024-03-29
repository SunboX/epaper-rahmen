#pragma once

#include <math.h>
#include <time.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "Record.h"
#include "Chart.h"

#define EPD_CS SS

// SPI pin definition
#define CS_PIN 15
#define RST_PIN 5
#define DC_PIN 4
#define BUSY_PIN 16

void drawLines();
void drawUpdatedTime(Record record);
void drawTemperatureBox(Record record);
void drawIntoScreen(Record record);
