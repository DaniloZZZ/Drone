/*
  ClientIfc.h - Library for Communicating with Client App
  Created by Danil Lykov 27 May, 2017
  Released into the public domain.
*/
#ifndef ClientIfc_h
#define ClientIfc_h

#include <NeoSWSerial.h>
#include "ClientData.h"
#include "Arduino.h"
#include <BasicLinearAlgebra.h>

class ClientIfc
{
  public:
    ClientIfc(NeoSWSerial *p);
    GetClient();
    SetDataSize(int size);
    ClientData Read();
    SendCommand(String in);
    SendMessage(String in);
    SendData(Matrix<6> data);
    Send(char c);
    print(String in);

    ClientData hist[5];
    bool connected;

  private:
    int sizeofDataWillBeSent;
    readuntil(char *buf,char *c);
    ParseData(char * in,float r[]);
    String substr(char * in,int s,int e);
    NeoSWSerial *port;
    int timeout;
    char endch;
    char commch;
    char msgch;
    char datach;
};

#endif