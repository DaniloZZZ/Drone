#include "MotorIfc.h"
#include "Arduino.h"
MotorIfc::MotorIfc(Servo *m)
{

  for (int i = 0; i < 4; i++)
  {
    mt[i] = m + i;
  }
  for (short i = 0; i < 5; i++)
  {
    for (short j = 0; j < 3; j++)
    {
      dhist[j][i] = 0;
    }
  }
  this->time = millis();
  //mt[0].attach(a);
  //mt[1].attach(b);
  //mt[2].attach(c);
  //mt[3].attach(d);
}
MotorIfc::SetMotors()
{
  int val;
  double dt;
  double anglemult = 35; //max angle=60deg/anglemult
  double lm = (0.0091 - 0.00015)*anglemult;
  double dm = 0.0181*anglemult;
  a0[0] = 0;
  a0[1] = 0;
  //Serial.print(a0[2]);
  // Serial.print("current vc:" + String(a[0]) + ";" + String(a[1]) + "  ");
      
  // Set historical data >>
  /*
  for (short i = 0; i < 5 - 1; i++)
  {
    for (short j = 0; j < 3; j++)
    {
     // dhist[j][i] = dhist[j][i + 1];
    }
  }*/
  //Serial.print("D error: ");
  /*
  for (short i = 0; i < 3; i++)
  {
    dhist[i][4] = (a[i] - a0[i]); //Setting to _current_ desired angle (!)
  //  Serial.print(String(dhist[i][4] - dhist[i][3]) + "; ");
  }*/

  // <<
  dt = (millis() - this->time);
  Serial.println(dt);
  this->time = millis();
  for (short i = 0; i < 2; i++)
  {
    v[i] = lm * (a[i] - a0[i]) + dm * (da[i]);
  }
  v[2] = -v[0];
  v[3] = -v[1];
  (*(this->dataForClient))(0) = v[0];
  (*(this->dataForClient))(1) = v[1];
  (*(this->dataForClient))(2) = dm * (da[0]);
  (*(this->dataForClient))(3) = dm * (da[1]);
  (*(this->dataForClient))(4) = lm * (a[0] - a0[0]) ;
  (*(this->dataForClient))(5) = lm * (a[1] - a0[1]) ;
  //val  = map(analogRead(1),0,1023,800,2300);
  //Serial.println(val);
  Serial.print("motor speeds: ");
  for (short i = 0; i < 4; i++)
  {
    val = (int)(map(a0[2]*0.73 + v[i], 0., 1., 800, 2300));

    mt[i]->writeMicroseconds(val);
    Serial.print(val);
    Serial.print(';');
  }
  //Serial.println(millis()-this->time);
}
MotorIfc::SetHeigh(double h)
{
  a0[2] = h;
}

MotorIfc::SetData(Matrix<6> *in)
{
  float X = (*in)(0);
  float Y = (*in)(1);
  float Z = (*in)(2);
  float rollrad = atan(X/(Z+1)); // calculated angle in radians
  float pitchrad = atan(Y/(Z+1)); // calculated angle in radians

  this->da[0] = -(*in)(4); // order is inversed, rotation over x causes y angle change
  this->da[1] = (*in)(3); //
  
  this->a[0] = rollrad;
  this->a[1] = pitchrad;
 // Serial.print("Before conv" + String(a[0]) + "; " + String(a[1]));
  Convertax(da);
  Convertax(a);
  this->da[2] = (*in)(5);
  this->a[2] = 1.0;
}
MotorIfc::Convertax(double *x)
{
  double tmp[3];
  double sq = 1 / 1.414213;
  tmp[0] = -(sq * x[0] + sq * x[1]);
  tmp[1] = -(-sq * x[0] + sq * x[1]);
  x[0] = tmp[0];
  x[1] = tmp[1];
}
MotorIfc::Calibrate()
{
  Serial.println("Calbrating motors");
  for (short i = 0; i < 4; i++)
  {
    mt[i]->writeMicroseconds(2300);
    delay(20);
  }
  delay(1000);
  Serial.println("Down");

  for (short i = 0; i < 4; i++)
  {
    mt[i]->writeMicroseconds(800);
    delay(20);
  }
  delay(2000);
}
double MotorIfc::map(double x, double i_min, double i_max, double o_min, double o_max)
{
  double y;
  y = (x - i_min) * (o_max - o_min) / (i_max - i_min) + o_min;
  if (y > o_max + 50)
    y = o_max + 50;
  if (y < o_min - 50)
    y = o_min - 50;
  return y;
}
