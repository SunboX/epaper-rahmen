#include "InternalSensor.h"

Adafruit_BME280 bme;

void initializeInternalSensor()
{
  unsigned status;

  status = bme.begin();

  if (!status) {
    while (1);
  }
}

void readInternalSensor(Record &record)
{
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  Serial.print("Pressure = ");

  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();

  record.indoorTemperature = bme.readTemperature();
  record.indoorHumidity = bme.readHumidity();
  record.indoorPressure = bme.readPressure() / 100.0F;
  record.indoorAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
}
