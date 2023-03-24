#include "Display.h"

// can use only half buffer size
//_BW<GxEPD2_750, GxEPD2_750::HEIGHT / 2> display(GxEPD2_750(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=5*/ 5, /*BUSY=16*/ 16));
GxEPD2_BW<GxEPD2_750, GxEPD2_750::HEIGHT> display(GxEPD2_750(/*CS=*/ 15, /*DC=*/ 27, /*RST=*/ 26, /*BUSY=*/ 25));
U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

void drawLines()
{
  display.drawFastHLine(20, 200, 599, GxEPD_BLACK);
  display.drawFastVLine(249, 20, 140, GxEPD_BLACK);
  display.drawFastVLine(459, 20, 140, GxEPD_BLACK);
}

void drawUpdatedTime(Record record)
{
  u8g2Fonts.setFont(u8g2_font_4x6_tf);

  struct tm tm;

  strptime(record.updatedAt.c_str(), "%Y-%m-%dT%H:%M:%S.%fZ", &tm);

  //ToDo: Use https://github.com/JChristensen/Timezone here
  tm.tm_hour += 1; // add 1 hous to the time
  mktime(&tm);     // normalize it

  char dateTime[20];

  strftime(dateTime, 20, "%d.%m.%Y | %H:%M", &tm);

  u8g2Fonts.setCursor(10, 10);
  u8g2Fonts.print("AKTUALISIERT: " + String(dateTime));
}

void drawIndoorTemperatureBox(Record record)
{
  float val = atof(record.indoorTemperature.c_str());
  int roundedVal = round(val);
  
  u8g2Fonts.setFont(u8g2_font_inb42_mf);

  u8g2Fonts.setCursor(70, 100);
  u8g2Fonts.print(String(roundedVal));
  u8g2Fonts.print(String(record.temp_unit));
}

void drawOudoorTemperatureBox(Record record)
{
  float val = atof(record.temp_value.c_str());
  int roundedVal = round(val);
  
  u8g2Fonts.setFont(u8g2_font_inb42_mf);

  u8g2Fonts.setCursor(285, 100);
  u8g2Fonts.print(String(roundedVal));
  u8g2Fonts.print(String(record.temp_unit));
}

void drawParticleBox(Record record)
{
  float val = atof(record.pm25_value.c_str());
  
  u8g2Fonts.setFont(u8g2_font_inb19_mf);

  u8g2Fonts.setCursor(475, 80);
  u8g2Fonts.print(String(val));
  u8g2Fonts.print(String(record.pm25_unit));

  val = atof(record.pm10_value.c_str());

  u8g2Fonts.setCursor(475, 120);
  u8g2Fonts.print(String(val));
  u8g2Fonts.print(String(record.pm10_unit));
}

void drawIntoScreen(Record record)
{
  Serial.println(F("display init"));

  display.init(115200); // uses standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  // *** special handling for Waveshare ESP32 Driver board *** //
  // ********************************************************* //
  SPI.end(); // release standard SPI pins, e.g. SCK(18), MISO(19), MOSI(23), SS(5)
  //SPI: void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
  SPI.begin(13, 12, 14, 15); // map and init SPI pins SCK(13), MISO(12), MOSI(14), SS(15)
  // *** end of special handling for Waveshare ESP32 Driver board *** //
  // **************************************************************** //

  u8g2Fonts.begin(display); // connect u8g2 procedures to Adafruit GFX
  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall

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

    drawIndoorTemperatureBox(record);
    drawOudoorTemperatureBox(record);
    drawParticleBox(record);

    for(int i = 0; i < 24; i++)
    {
      Serial.print("v ");
      Serial.print(i);
      Serial.print(" ");
      Serial.println(record.data_24h_temp_history[i]);
    }
    
    Chart::draw(
      display,
      20, // xPos
      220, // yPos
      5, // xPadding
      0, // yPadding
      130, // height
      210, // width
      record.data_24h_temp_history,
      record.data_24h_temp_history_labels,
      24 // arrayLength
    );

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
