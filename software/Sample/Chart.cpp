#include "Chart.h"

Chart::Chart(GxEPD_Class display) {
    display = display;
}

void Chart::draw(
    uint16_t xPadding,
    uint16_t yPadding,
    uint16_t height,
    uint16_t width,
    JsonArray &dataset,
    JsonArray &labels)
{
    // Draw the axises
    display.drawFastVLine(xPadding, 0, height - yPadding, GxEPD_BLACK);
    display.drawFastHLine(xPadding, height - yPadding, width, GxEPD_BLACK);

    // Draw the X value texts
    for (uint16_t i = 0; i < (sizeof(labels) / 2); i++)
    {
        uint16_t x = getXPixel(i, dataset, labels, width, height, xPadding, yPadding);
        uint16_t y = height - yPadding + 15 + i;

        //display.drawPixel(x, y, GxEPD_BLACK);

        const char *label = labels[i];

        display.setCursor(x, y);
        display.print(label);
    }

    // Draw the Y value texts
    int breakperiod = 50; // chartData.ybreakperiod;
    for (uint16_t i = 0; i <= getMaxY(dataset); i += breakperiod)
    {
        uint16_t x = xPadding - 20;
        uint16_t y = getYPixel(i, dataset, labels, width, height, xPadding, yPadding);

        display.setCursor(x, y);
        display.print(i);
    }

    // Draw the graph points
    for (uint16_t i = 0; i < (sizeof(dataset) / 2); i++)
    {
        uint16_t x = getXPixel(i, dataset, labels, width, height, xPadding, yPadding);
        uint16_t y = getYPixel(dataset[i], dataset, labels, width, height, xPadding, yPadding);

        display.fillCircle(x, y, 3, GxEPD_BLACK);
    }

    // Draw the line graph
    for (uint16_t i = 1; i < (sizeof(dataset) / 2); i++)
    {
        uint16_t x1 = getXPixel(i - 1, dataset, labels, width, height, xPadding, yPadding);
        uint16_t y1 = getYPixel(dataset[i - 1], dataset, labels, width, height, xPadding, yPadding);
        uint16_t x2 = getXPixel(i, dataset, labels, width, height, xPadding, yPadding);
        uint16_t y2 = getYPixel(dataset[i], dataset, labels, width, height, xPadding, yPadding);

        display.drawLine(x1, y1, x2, y2, GxEPD_BLACK);
    }

    display.update();
}

// Returns the max Y value in our data list
uint16_t getMaxY(JsonArray &dataset)
{
    uint16_t max = 0;
    for (uint16_t i = 0; i < sizeof(dataset); i++)
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
uint16_t getXPixel(uint16_t val, JsonArray &dataset, JsonArray &labels, uint16_t width, uint16_t height, uint16_t xPadding, uint16_t yPadding)
{
    return xPadding + ((width - xPadding) / (sizeof(labels) / 2)) * val;
}

// Return the y pixel for a graph point
uint16_t getYPixel(uint16_t val, JsonArray &dataset, JsonArray &labels, uint16_t width, uint16_t height, uint16_t xPadding, uint16_t yPadding)
{
    return height - (((height - yPadding) / getMaxY(dataset)) * val) - yPadding;
}
