#include "Chart.h"

void Chart::draw(
  GxEPD2_BW<GxEPD2_750, GxEPD2_750::HEIGHT> &display,
  uint16_t xPos,
  uint16_t yPos,
  uint16_t xPadding,
  uint16_t yPadding,
  uint16_t height,
  uint16_t width,
  float dataset[],
  int labels[],
  uint16_t arrayLength
) {
  // Draw the axises
  display.drawFastVLine(xPos + xPadding, yPos, height - yPadding, GxEPD_BLACK);
  display.drawFastHLine(xPos + xPadding, yPos + height - yPadding, width, GxEPD_BLACK);

  // Draw the X value texts
  for (int i = 0; i < arrayLength; i += 2)
  {
    uint16_t x = xPos + Chart::getXPixel(i, dataset, labels, arrayLength, width, height, xPadding, yPadding);
    uint16_t y = yPos + height - yPadding + 15;

    //display.drawPixel(x, y, GxEPD_BLACK);

    //const char *label
    int label = labels[i];

    Serial.print("label ");
    Serial.println(label);

    display.setCursor(x, y);
    display.print(label);
  }

  // Draw the Y value texts
  int breakperiod = 2; // chartData.ybreakperiod;
  for (uint16_t i = 0; i <= Chart::getMaxY(dataset, arrayLength); i += breakperiod)
  {
    uint16_t x = xPos + xPadding - 20;
    uint16_t y = yPos + Chart::getYPixel(i, dataset, labels, arrayLength, width, height, xPadding, yPadding);

    display.setCursor(x, y);
    display.print(i);
  }

  // Draw the graph points
  for (uint16_t i = 0; i < arrayLength; i++)
  {
    uint16_t x = xPos + Chart::getXPixel(i, dataset, labels, arrayLength, width, height, xPadding, yPadding);
    uint16_t y = yPos + Chart::getYPixel(dataset[i], dataset, labels, arrayLength, width, height, xPadding, yPadding);

    display.fillCircle(x, y, 3, GxEPD_BLACK);
  }

  // Draw the line graph
  for (uint16_t i = 1; i < arrayLength; i++)
  {
    uint16_t x1 = xPos + Chart::getXPixel(i - 1, dataset, labels, arrayLength, width, height, xPadding, yPadding);
    uint16_t y1 = yPos + Chart::getYPixel(dataset[i - 1], dataset, labels, arrayLength, width, height, xPadding, yPadding);
    uint16_t x2 = xPos + Chart::getXPixel(i, dataset, labels, arrayLength, width, height, xPadding, yPadding);
    uint16_t y2 = yPos + Chart::getYPixel(dataset[i], dataset, labels, arrayLength, width, height, xPadding, yPadding);

    display.drawLine(x1, y1, x2, y2, GxEPD_BLACK);
  }
}

// Returns the max Y value in our data list
uint16_t Chart::getMaxY(float dataset[], uint16_t arrayLength)
{
  uint16_t max = 0;
  for (uint16_t i = 0; i < arrayLength; i++)
  {
    if (dataset[i] > max)
    {
      max = dataset[i];
    }
  }
  max += 10 - max % 10;
  return max;
}

// Return the x pixel for a graph point
uint16_t Chart::getXPixel(uint16_t val, float dataset[], int labels[], uint16_t arrayLength, uint16_t width, uint16_t height, uint16_t xPadding, uint16_t yPadding)
{
  return xPadding + ((width - xPadding) / arrayLength) * val;
}

// Return the y pixel for a graph point
uint16_t Chart::getYPixel(uint16_t val, float dataset[], int labels[], uint16_t arrayLength, uint16_t width, uint16_t height, uint16_t xPadding, uint16_t yPadding)
{
  return height - (((height - yPadding) / Chart::getMaxY(dataset, arrayLength)) * val) - yPadding;
}
