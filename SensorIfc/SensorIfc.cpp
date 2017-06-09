
#include "Arduino.h"
#include "SensorIfc.h"
#include "Tools.h"
#include "ADXL345.h"
#include "Wire.h"
#include <BasicLinearAlgebra.h>

//TODO: having your 16MHz processor stuck in a busy loop waiting on a 100kHz
// communication stream is not efficient. You may be better off writing your own I2C code.

SensorIfc::SensorIfc()
{
    this->filterK = 0.6;
    this->timeOfPrev = millis();
}
SensorIfc::init()
{
    configueAccel();
    AccData.Fill(0);
    oldAccData.Fill(0);

    configureHyro();
}
SensorIfc::configueAccel()
{
    this->Accel = ADXL345();
    // Assign an interrupt for free fall situation;
    // TODO: inmpement my own function to send all this stuff at once, not opening closing connections everytime
    Accel.setFreeFallThreshold(0x06); // 375 mg
    Accel.setFreeFallDuration(0x04);  // 350 ms

    Accel.getInterruptSource();      // Clear previously set interrupts if left occasionally
    Accel.setInterrupt(2, 0);        // Disable interupt to configure stuff
    Accel.setInterruptLevelBit(0);   // Interrupt pin will be high on event
    Accel.setInterruptMapping(2, 0); // set free fall bit
    Accel.setInterrupt(2, 1);        // Enable firing an event when free fall
    Accel.powerOn();                 // Start measuring data
}
SensorIfc::configureHyro()
{
    float g[3][1];
    this->Gyro = ITG3200();
    this->Gyro.init(ITG3200_ADDR_AD0_LOW);
    delay(10);

    // setting offset just with one spot measurment
    this->Gyro.readGyro(g[0]);
    this->offsetGyro = g;
    // Serial.print("zero Calibrating...");
    // gyro.zeroCalibrate(2500, 2);
    // TODO: configure low-pass filtrer settings (?)
}
SensorIfc::getTemp()
{
    float temp;
    Gyro.readTemp(&temp);
    return temp;
}
void SensorIfc::rawRead()
{
    int a[3][1];
    float g[3][1]; //Matrix class supports only 2d arrays on assignment
    this->Accel.readAccel(a[0]);
    this->Gyro.readGyro(g[0]);
    this->AccData = a;
    this->GyroData = g;
}
Matrix<6> SensorIfc::Read()
{
    Matrix<3> angles;
    Matrix<3> grate;
    Matrix<1> K;
    Matrix<1> rawConvert;
    unsigned long looptime;
    this->rawRead();

    K.Fill(filterK);
    rawConvert.Fill(1 / 256.0);
    angles = this->AccData;
    //Serial.println(angles(0));
    //Serial.println(AccData(0));

    //Filtering data
    angles = (angles + this->oldAccData) * K - this->oldAccData;
    this->oldAccData = angles;

    rawConvert(0) = 1 / 14.375;
    grate = (this->GyroData - this->offsetGyro) * rawConvert;
    //Filtering data
    grate = (grate + this->oldGyroData) * K - this->oldGyroData;
    this->oldGyroData = grate;

    looptime = millis() - this->timeOfPrev;
    this->timeOfPrev = millis();
    Serial.print("Loop time= ");
    Serial.println(looptime);
    Matrix<6> res = VertCat(angles,grate); // concat the results
    return  res;
}
SensorIfc::Calibrate(int k)
{
    SensorData s; // sum of data
    SensorData r; // current data variable
    double max;
    double min = 1000;
    Serial.println("Started Calibration");
    for (int i = 0; i < k; i++)
    {
        // r = rawRead();
        s = s + r;
        if (max < r.maxval())
            max = r.maxval();
        if (min > r.minval())
            min = r.minval();
    }
    s = s / k; // mean of k data
    calib = s;
    Serial.println("Calibration done: Mean: " + s.ToString() +
                   "max=" + String(max) +
                   "min=" + String(min) + ";");
}
