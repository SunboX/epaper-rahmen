#pragma once

struct Record
{
  String updatedAt;
  String temp_sensor_id;
  String temp_value;
  String temp_unit;
  String pm10_sensor_id;
  String pm10_value;
  String pm10_unit;
  String pm25_sensor_id;
  String pm25_value;
  String pm25_unit;
  float data_24h_temp_history[24];
  int data_24h_temp_history_labels[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  float indoorTemperature;
  float indoorHumidity;
  float indoorPressure;
  float indoorAltitude;
};
