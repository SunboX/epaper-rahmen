#ifndef Chart_h
#define Chart_h

#include "Arduino.h"
#include <GxEPD.h>
#include <GxGDEW075T8/GxGDEW075T8.cpp>
#include "pf_tempesta_seven5pt7b.h"
#include <ArduinoJson.h>

class Chart
{
  public:
    Chart(GxGDEW075T8&);
    void draw(
        uint16_t,
        uint16_t,
        uint16_t,
        uint16_t,
        JsonArray,
        JsonArray
    );

  private:
    GxGDEW075T8 display;
};

#endif
