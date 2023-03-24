#pragma once

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <ArduinoJson.h>

class Chart
{
  public:
    static void draw(
      GxEPD2_BW<GxEPD2_750, GxEPD2_750::HEIGHT> &display,
      uint16_t xPos,
      uint16_t yPos,
      uint16_t,
      uint16_t,
      uint16_t,
      uint16_t,
      float[], //JsonArray&,
      int labels[], //JsonArray&
      uint16_t arrayLength
    );

  private:
    static uint16_t getMaxY(
      float[],
      uint16_t arrayLength
    );

    static uint16_t getXPixel(
      uint16_t val,
      float dataset[],
      int labels[],
      uint16_t arrayLength,
      uint16_t width,
      uint16_t height,
      uint16_t xPadding,
      uint16_t yPadding
    );

    static uint16_t getYPixel(
      uint16_t val,
      float dataset[],
      int labels[],
      uint16_t arrayLength,
      uint16_t width,
      uint16_t height,
      uint16_t xPadding,
      uint16_t yPadding
    );

};
