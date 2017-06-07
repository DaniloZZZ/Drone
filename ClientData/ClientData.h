/*
  ClientData.h - Class for representing data form Client
  Created by Danil Lykov 27 May, 2017
  Released into the public domain.
*/
#ifndef ClientData_h
#define ClientData_h

#include "Tools.h"
class ClientData
{
  public:
    ClientData();
    String type;
    String command;
    String msg;
    SensorData data;
    String raw;
};

#endif

