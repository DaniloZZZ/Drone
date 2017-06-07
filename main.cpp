#include <Tools.h>

#include <MemoryFree.h>
#include <NeoSWSerial.h>

#include "Servo.h"
#include <SensorIfc.h>
#include <ClientIfc.h>
#include <MotorIfc.h>
/*
Explanations:
CONCEPTIONS:
  Client - Andriod app running on phone
  Protocol - a set of rules we use to pack data that Client will understand
  ESP8266 - Wi-Fi module, every time gets data transmits to Client same in other direction
INTERFACES:
  ClientIfc - A class for communicating with client, takes care of senfing data according to Protocol
  MotorIfc - A class for setting speed motors
SENSORS:
  ADXL345 - Accelerometer, see SensorIfc for details. Library folder: ADXL345_library
  ITG3200 - Gyroscope, see SensorIfc for details. Library folder: ITG3200
  HMC5883 - magnetometer. Library folder: HMC5883_library

*/


NeoSWSerial mySerial(8, 9); // RX, TX // Library for communicating with ESP8266
ClientIfc client(&mySerial);
ClientData in;

Servo mt[4];
MotorIfc motor(mt);
SensorIfc sensor; // Sensor uses A4, A5 for i2c communication with GY-85
SensorData data;

bool wasint; // used to check if there was an interrupt to read INT_SOURCE and clear INT_FF bit in Accel

void setup() {
  Serial.begin(115200);
  mySerial.begin(38400);

  Serial.println(F("Goodnight moon!"));
  pinMode(7, OUTPUT);


  MotorI(3, 4, 5, 6);
  motor.Calibrate();

  Serial.println(F("Waiting for Client to answer"));
  client.GetClient();
  client.SendCommand("WAIT");

  sensor.init();
  wasint = false;
  attachInterrupt(0, fire, HIGH);   // attach interrupt from accel to arduino
  sensor.Calibrate(200);
}
void loop() {
  clearInterrupt();
  
  data = sensor.Read();
  motor.SetData(&data);
  motor.SetMotors();

  while (Serial.available()) client.Send(Serial.read());
  delay(5);
  // Serial.print(F("\nArduino::send>> " ));
  // data.toChar(str);
  // Serial.println(str);
  
  client.SendData(&data);
  in = client.Read();
  //Serial.print(F("Got from client: "));
  //Serial.println(in.raw);
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  delay(5);
  
  if (in.type == "command") { 
    HandleCommands(in.command);
  } else if (in.type == "msg") {
    Serial.println("Got Message:" + in.msg);
  } else if (in.type == "data") {
    Serial.print(F("Parsed Data: "));
    Serial.println(in.data.y);
    motor.SetHeigh((abs(in.data.y)) / 8.);
    analogWrite(6, map(in.data.y, -2, 10, 0, 255));
    delay(1);
  }
}

void fire()
{
  if (!wasint) {
    wasint = true;
    digitalWrite(7, HIGH); // This sets 5v on 7th pin, blink a led
    Serial.println("We're falling!");
    // Note: we can't use i2c while handling interrupts because i2c uses interrupts. That's why we use clearInterrupt()
  }
}
void clearInterrupt() { // clear interrupt bit from INT_SOURCE register to stop continiously firing interrupts on arduino
  if (wasint) {
    byte a = 0;
    a  = sensor.Accel.getInterruptSource();
    digitalWrite(7, LOW);
    Serial.print("Int Source: ");
    Serial.println(a, BIN); // Interrupt source Dx - number of bit: (D7,D1,D0 are set even interrupt wasn't assigned to them)
    /*D7          D6         D5         D4      D3          D2         D1         D0
      DATA_READY SINGLE_TAP DOUBLE_TAP Activity Inactivity FREE_FALL  Watermark  Overrun*/
    wasint = false; // We're now ready to handle new interrupts
  }
}

void HandleCommands(String com){
      if (com == "BREAK") {
      Serial.println( "Disconnecting"); \
      motor.SetHeigh(-0.2);
      analogWrite(6, 0 );
      client.GetClient();
    }
  }

void   waitforEsp() {
  Serial.println("Waiting for ESP");
  String k = "";
  char ch[4];
  char t;
  ch[3] = 0x00;
  while (k != "end") {
    if (mySerial.available()) {
      t = mySerial.read();
      Serial.write(t);

      for (int i = 0; i < 2; i++) {
        ch[i] = ch[i + 1];
      }
      ch[2] = t;
      k = String(ch);
    }
  }
  Serial.println("\nArduino::ESPready!");
}
void MotorI(short a, short b, short c, short d)
{
  for (int i = 0; i < 4; i++)
  {
    mt[i] = Servo();
  }
  mt[0].attach(a);
  mt[1].attach(b);
  mt[2].attach(c);
  mt[3].attach(d);
}