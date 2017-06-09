
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
    filterK = 0.6;
}
SensorIfc::init()
{   
    configueAccel();
    AccData.Fill(0);
    oldAccData.Fill(0);

    configureHyro(); 

    // Set size of Data buffer for filtering;
    filterDataSize = 1;
    buf = new SensorData[filterDataSize];
}
SensorIfc::configueAccel(){
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
SensorIfc::configureHyro(){
    this->Gyro = ITG3200();
    this->Gyro.init(ITG3200_ADDR_AD0_LOW);
    // TODO: configure low-pass filtrer settings (?)
}
SensorIfc::getTemp(){
    float temp;
    Gyro.readTemp(&temp);
    return temp;
}
void SensorIfc::rawRead()
{
    int x,y,z;
    Accel.readAccel(&x, &y, &z);
    AccData(0)=x;
    AccData(1)=y;
    AccData(2)=z;
}
SensorData SensorIfc::Read(){
    this->rawRead();
    Matrix<3> angles;
    Matrix<1> K;
    Matrix<1> mK;    
    K.Fill(filterK);
    mK.Fill(1.0-filterK);
    angles = this->AccData*K + this->oldAccData*mK;
    this->oldAccData = angles; 
    //rollrad = atan(Y / sqrt( angles(0)* X + Z * Z)); // calculated angle in radians
    //pitchrad = atan(X / sqrt(Y * Y + Z * Z)); // calculated angle in radians
    SensorData out(angles(0),angles(1),angles(2));
    return out;
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
