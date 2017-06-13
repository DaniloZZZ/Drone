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
#include <BasicLinearAlgebra.h>
class MotorIfc
{
public:
  MotorIfc(Servo *m);
  SetData(Matrix<6> *in);
  SetAngle();
  Calibrate();
  String ToString();
  SetMotors();
  SetHeigh(double h);
  Matrix<6> * dataForClient;
  Clear();
  double a[3]; //angle
  double da[3]; // deriv of angle
  double a0[3]; // desired angle

private:
  unsigned int time; 
  double dhist[3][5];
  double map(double x, double i_min, double i_max,
             double o_min, double o_max);
  Servo *mt[4];
  double v[4];
  Convertax(double *x);
};

#endif