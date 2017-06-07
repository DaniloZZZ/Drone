
/****************
* SENSORS DESCRIPTIONS:
*****************/

// ACCELEROMETER ADXL345  http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf
// Measures acceleration, u can get direction of g-vector
/*
    Features:
    - Tap/Double Tap detection
    - Free-Fall detection
    - Several measurment ranges: +-2, 4, 6, 8 g on each axis
    - Set offset values to x,y,z. Addresses: 0x1E, 0x1F, 0x20
    - 32 level output data FIFO minimizes host processor load
    - Built in motion detection functions 
    - Activity/Inactivity monitoring
 
    Sensivity: 256 bytes/g  (g = 9.81m/s2)
    Noize: 1 byte;
    i2c: 100-400 hz
*/

// GYROSCOPE ITG3200 https://www.sparkfun.com/datasheets/Sensors/Gyro/PS-ITG-3200-00-01.4.pdf
// Measures oriantation change rate ( °/sec )
/*
    Features:
    - Digitally-programmable low-pass filter 
    - Digital-output temperature sensor 
    - No high pass filter needed
    - Optional external clock inputs of 32.768kHz or 19.2MHz to synchronize with system clock
    
    Sensivity: 14.375 LSBs per °/sec and a full-scale range of ±2000°/sec
    Turn on time: 50ms
    i2c: up to 400 hz
*/

#include "Arduino.h"
#include "SensorIfc.h"
#include "Tools.h"
#include "ADXL345.h"
#include "Wire.h"

//TODO: having your 16MHz processor stuck in a busy loop waiting on a 100kHz
// communication stream is not efficient. You may be better off writing your own I2C code.

SensorIfc::SensorIfc()
{
}
SensorIfc::init()
{

    Accel = ADXL345();
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

    //Set Arduino pin modes to INPUT
    // pinMode(pinx, INPUT);
    //  pinMode(piny, INPUT);
    //   pinMode(pinz, INPUT);
    //Assign numbers of pins to local variables
    // this->pinx = pinx;
    //  this->piny = piny;
    //   this->pinz = pinz;

    // Set size of Data buffer for filtering;
    filterDataSize = 1;
    buf = new SensorData[filterDataSize];
}

SensorData SensorIfc::rawRead()
{
    return SensorData((float)analogRead(pinx),
                      (float)analogRead(piny),
                      (float)analogRead(pinz));
}
SensorData SensorIfc::Read()
{
    rawdata = rawRead(); // get raw data
    SensorData res;

    for (int i = 0; i < filterDataSize - 1; i++) // push in buffer
    {
        buf[i] = buf[i + 1];
        res = res + buf[i]; // add sensor data
    }
    buf[filterDataSize - 1] = rawdata;
    res = res + rawdata;
    res = res / filterDataSize - calib;
    this->data = res;
    return res;
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
        r = rawRead();
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
