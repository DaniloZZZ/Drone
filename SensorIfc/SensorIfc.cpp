
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
    this->timeOfPrev = millis();
}
SensorIfc::SetFilterKPointer(float * k){
    this->filterK = k;
}
SensorIfc::init()
{
    configueAccel();
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
    delay(10);
}
SensorIfc::configureHyro()
{
    this->Gyro = ITG3200();
    this->Gyro.init(ITG3200_ADDR_AD0_LOW);
    delay(10);

    // setting offset just with one spot measurment
    // Serial.print("zero Calibrating...");
    // gyro.zeroCalibrate(2500, 2);
    // TODO: configure low-pass filtrer settings (?)
}
SensorIfc::Calibrate(int k)
{
    Matrix<3> gsum = {0,0,0};
    Matrix<3> asum = {0,0,0};

    Serial.println("Started Calibration");
    for (int i = 0; i < k; i++)
    {
        this->rawRead();
        asum+=this->AccData;
        gsum+=this->GyroData;
    }
    this->offsetAccel = asum *(1/(float)k); // mean of k data
    this->offsetGyro = gsum*(1/(float)k);
    Serial<<"Calibration done: " << AccData << ";\n";
}

SensorIfc::getTemp()
{
    float temp;
    Gyro.readTemp(&temp);
    return temp;
}
void SensorIfc::rawRead()
{
    int a[3];
    float g[3];
    this->Accel.readAccel(a);
    this->Gyro.readGyro(g);
    for(byte i=0;i<3;i++){
        this->AccData(i) = (float)a[i];
        this->GyroData(i) = g[i];
    }
}

Matrix<SENSOR_DATA_SIZE> SensorIfc::Read()
{
    Matrix<3> angles;
    Matrix<3> grate;
    float rawConvert;
    unsigned long looptime;
    this->rawRead();

    angles = (this->AccData - this->offsetAccel)*(1/256.f);
    //Serial.println(angles(0));
    //Serial.println(AccData(0));

    //Filtering data
    angles = (angles - this->oldAccData) * (*filterK) + this->oldAccData;
    this->oldAccData = angles;

    rawConvert = 1 / 14.375/ 180 * 3.14159;  // to radians
    grate = (this->GyroData - this->offsetGyro) * rawConvert;
    //Filtering data
    grate = (grate - this->oldGyroData) * (*filterK) + this->oldGyroData;
    this->oldGyroData = grate;

    looptime = millis() - this->timeOfPrev;
    this->timeOfPrev = millis();
    Serial.print("Loop= ");
    Serial.println(looptime);
    //Matrix<SENSOR_DATA_SIZE> res = angles,grate); // concat the results
    return  angles&&grate;
}
