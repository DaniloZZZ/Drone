#include "Tools.h"

SensorData::SensorData(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
SensorData::SensorData()
{
    this->x = 0;
    this->y = 0;
    this->z = 0;
}
SensorData SensorData::operator+(const SensorData &b)
{
    SensorData c;
    c.x = this->x + b.x;
    c.y = this->y + b.y;
    c.z = this->z + b.z;
    return c;
}
SensorData SensorData::operator-(const SensorData &b)
{
    SensorData c;
    c.x = this->x - b.x;
    c.y = this->y - b.y;
    c.z = this->z - b.z;
    return c;
}
SensorData SensorData::operator/(const float &b)
{
    SensorData c;
    c.x = this->x / b;
    c.y = this->y / b;
    c.z = this->z / b;
    return c;
}
SensorData::operator=(const SensorData &D)
{
    x = D.x;
    y = D.y;
    z = D.z;
}
float SensorData::maxval()
{
    float r = x;
    if (y > r)
        r = y;
    if (z > r)
        r = z;
    return r;
}
float SensorData::minval(){
    float r = x;
    if (y < r)
        r = y;
    if (z < r)
        r = z;
    return r;
}
SensorData::toChar(char *str){
    char tmp[8];
    dtostrf(x,3,2,tmp);
    sprintf(str,"ax: %s;",tmp);
        dtostrf(y,3,2,tmp);
    sprintf(str,"%s ay: %s;",str,tmp);
        dtostrf(z,3,2,tmp);
    sprintf(str,"%s az: %s;",str,tmp);
}
String SensorData::ToString()
{
    return String("ax: ") + String(x) + "; ay: " +
           String(y) + "; az: " +
           String(z) + ";";
}
SensorData::setVals(float x,float y,float z){
    this->x = x;
    this->y = y;
    this->z = z;
}

