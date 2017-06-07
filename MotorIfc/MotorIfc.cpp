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

  //mt[0].attach(a);
  //mt[1].attach(b);
  //mt[2].attach(c);
  // mt[3].attach(d);
}
MotorIfc::SetMotors()
{
  int val;

  double lm = 0.007;
  double dm = 0.0050;
  g[0] = 0;
  g[1] = 0;
  Serial.print(g[2]);
 // Serial.print("current vc:" + String(a[0]) + ";" + String(a[1]) + "  ");
  
  // Set historical data >>
  for (short i = 0; i < 5 - 1; i++)
  {
    for (short j = 0; j < 3; j++)
    {
      dhist[j][i] = dhist[j][i + 1];
    }
  }
  //Serial.print("D error: ");

  for (short i = 0; i < 3; i++)
  {
    dhist[i][4] = (a[i] - g[i]); //Setting to _current_ desired angle (!)
  //  Serial.print(String(dhist[i][4] - dhist[i][3]) + "; ");
  }

  // <<

  for (short i = 0; i < 2; i++)
  {
    v[i] = lm * (a[i] - g[i]) + dm * (dhist[i][4] - dhist[i][3]);
  }
  v[2] = -v[0];
  v[3] = -v[1];

  //val  = map(analogRead(1),0,1023,800,2300);
  //Serial.println(val);
  //Serial.print("motor speeds: ");
  for (short i = 0; i < 4; i++)
  {
    val = (int)(map(g[2] + v[i], 0., 2., 800, 2300));

    mt[i]->writeMicroseconds(val);
    //Serial.print(String(val) + "; ");
  }
 // Serial.print("\n ");
}
MotorIfc::SetHeigh(double h)
{
  g[2] = h;
}

MotorIfc::SetData(SensorData *in)
{

  this->a[0] = in->x;
  this->a[1] = in->y;
 // Serial.print("Before conv" + String(a[0]) + "; " + String(a[1]));
  Convertax(a);
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
