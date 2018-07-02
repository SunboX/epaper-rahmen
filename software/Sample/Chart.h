#ifndef Chart_h
#define Chart_h

#include "Arduino.h"
#include <GxEPD.h>
#include "pf_tempesta_seven5pt7b.h"
#include <ArduinoJson.h>

class Chart
{
  public:
    Chart(GxEPD_Class display);
    void draw(
        uint16_t xPadding,
        uint16_t yPadding,
        uint16_t height,
        uint16_t width,
        JsonArray &dataset,
        JsonArray &labels
    );

  private:
    GxEPD_Class display;
};

#endif
