#pragma once

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "Record.h"

#define SEALEVELPRESSURE_HPA (1013.25)

void initializeInternalSensor();
void readInternalSensor(Record &record);
