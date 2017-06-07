/*
  SensorData.h - Library for Data read from Drone Sensors
  Created by Danil Lykov 26 May, 2017
  Released into the public domain.
*/
#ifndef SensorData_h
#define SensorData_h

#include "Arduino.h"
class SensorData
{
public:
  SensorData(int x, int y, int z);
  SensorData(float x, float y, float z);
  SensorData();
  setVals(float x, float y, float z);
  SensorData operator+(const SensorData &b);
  SensorData operator-(const SensorData &b);
  SensorData operator/(const float &b);

  operator=(const SensorData &b);
  float maxval();
  float minval();

  String ToString();
  toChar(char *str);
  Clear();
  float x, y, z;
};

#endif

#ifndef Debug_h
#define Debug_h

//#define DEBUG

#include "Arduino.h"
class Debug
{
public:
  static void msg(char *m)
  {
      //  Serial.println("hi
  #ifdef DEBUG
    Serial.print("Debug:: ");
    Serial.print(m);
    Serial.print("\n");
    delay(5);
#endif 
  }
};

#endif