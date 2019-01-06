#include "Display.h"

// can use only half buffer size
GxEPD2_BW<GxEPD2_750, GxEPD2_750::HEIGHT / 2> display(GxEPD2_750(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=5*/ 5, /*BUSY=16*/ 16));

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

void drawLines()
{
  display.drawLine(20, 200, 619, 200, GxEPD_BLACK);
  display.drawLine(249, 20, 249, 160, GxEPD_BLACK);
  display.drawLine(459, 20, 459, 160, GxEPD_BLACK);
}

void drawUpdatedTime(Record record)
{
  u8g2Fonts.setFont(u8g2_font_4x6_tf);

  struct tm tm;

  strptime(record.updatedAt.c_str(), "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

  tm.tm_hour += 2; // add 2 hous to the time
  mktime(&tm);     // normalize it

  char dateTime[20];

  strftime(dateTime, 20, "%d.%m.%Y | %H:%M", &tm);

  u8g2Fonts.setCursor(10, 10);
  u8g2Fonts.print("AKTUALISIERT: " + String(dateTime));
}

void drawTemperatureBox(Record record)
{
  u8g2Fonts.setFont(u8g2_font_inb27_mf);

  u8g2Fonts.setCursor(50, 100);
  u8g2Fonts.print(String(record.temp_value));
  u8g2Fonts.print(String(record.temp_unit));
}

void drawIntoScreen(Record record)
{
  yield();

  Serial.println(F("display init"));

  display.init(115200);

  yield();

  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX
  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

  yield();

  display.setTextColor(GxEPD_BLACK);

  u8g2Fonts.setFontMode(1);                  // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);             // left to right (this is default)
  u8g2Fonts.setForegroundColor(GxEPD_BLACK); // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(GxEPD_WHITE); // apply Adafruit GFX color

  uint16_t x = (display.width() - 160) / 2;
  uint16_t y = display.height() / 2;

  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    drawLines();

    drawUpdatedTime(record);

    drawTemperatureBox(record);

  } while (display.nextPage());
}

/*
void drawTemperatureChart()
{
  u8g2Fonts.setFont(u8g2_font_inb27_mf);

  u8g2Fonts.setCursor(50, 300);
  u8g2Fonts.print(String(temp_value));
  u8g2Fonts.print(String(temp_unit));
}
*/
