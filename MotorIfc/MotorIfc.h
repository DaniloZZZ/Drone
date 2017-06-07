/*
  MotorIfc.h - Library for Communicating with Drone Motors
  Created by Danil Lykov 27 May, 2017
  Released into the public domain.
*/
#ifndef MotorIfc_h
#define MotorIfc_h

#include <Servo.h>
#include "Arduino.h"
#include "Servo.h"
#include "Tools.h"
class MotorIfc
{
public:
  MotorIfc(Servo *m);
  SetData(SensorData *in);
  SetAngle();
  Calibrate();
  String ToString();
  SetMotors();
  SetHeigh(double h);

  Clear();
  double a[3];
  double g[3];

private:
  double dhist[3][5];
  double map(double x, double i_min, double i_max,
             double o_min, double o_max);
  Servo *mt[4];
  double v[4];
  Convertax(double *x);
};

#endif