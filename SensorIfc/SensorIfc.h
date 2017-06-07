/*
  SensorIfc.h - Interface for Sensors of Drone
  Created by Danil Lykov 26 May, 2017
  Released into the public domain.
*/


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
    - Set offset values to x,y,z: 0x1E, 0x1F, 0x20
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
    i2c: 400 hz
*/

#ifndef SensorIfc_h
#define SensorIfc_h

#include <ADXL345.h>
#include "Tools.h"
#include "HardwareSerial.h"
class SensorIfc
{
  public:
    //SensorIfc(int pinx, int piny, int pinz);
    SensorIfc();
    init();
    Calibrate(int k);
    ADXL345 Accel;
    SensorData Read();
    SensorData rawRead();
    SensorData data;
    SensorData rawdata;
    SensorData calib;

  private:
    SensorData * buf;
    int filterDataSize;
    int pinx, piny, pinz;
};

#endif