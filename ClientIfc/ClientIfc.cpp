
/***********
 DESCTIPTION OF PROTOCOL
************/
/*
Establishing connection:
    Network communication starts with checking if there's some data from ESP in buffer.
    If there's nothing, it sends connect signal. If there's something, Arduino prints it to PC via Serial.
    If ESP is ok, it will answer with IP address and tell you if it has a client connected.
    Then ESP transmitts the connect signal to Client, which starts to send data if recieves that signal.
Sending data:
    Every data starts with a char indicating type of data and ends with endchar. 
    After sending connect sifnal Arduino goes to infinite loop and sends data from sensors to ESP.
    ESP will wait for end char from arduino and transmit info to client.
Recieving data:
    Anytime ESP gets a char from client it sends it to Arduino.
    After sending sensor data, Arduino starts listening to ESP and collecting all chars in buffer. 
    It will go to data parsing when recieves an endchar from ESP or to the next loop iteration after timeout.
*/
#include "Tools.h"
#include "ClientIfc.h"
#include "Arduino.h"
#include "string.h"

ClientIfc::ClientIfc(NeoSWSerial *p)
{
    // defining chars used as protocol of communicating
    endch = '$';
    commch = '>';
    msgch = '!';
    datach = '#';

    // Assign sonnection object to local var
    this->port = p;
    connected = false;

    // set how long to wait for endchar
    timeout = 50;
    this->port->setTimeout(timeout);
}
ClientIfc::GetClient()
{
    Debug::msg("Create client");
    // use to determine if esp is on
    bool esp = false;

    // store input
    String in = "";
    char c;
    // wait for data from port forewer
    // TODO: Set timeout(?)
    while (true)
    {
        if (port->available())
        {// if got some data, esp is on, read data
            esp = true;
            c = port->read(); // read data from port
            Serial.print(c); // print all of info, also service info from esp (e.g. ipaddr)
            delay(1);
            if (c == '$')
            {// The convention: client sends OK msg from start of connection till drone answers
                if (in == "!OK") 
                    break;
                in = "";
            }
            else
                in += c;
        }
        else
        {
            if (!esp)
            { // it's for avoid sending "connect" multiple times
                SendCommand("connect");
                delay(30);
            }
        }
    }
    SendCommand("OK");
    connected = true;
}

ClientIfc::print(String in)
{
    port->println(in);
}
ClientData ClientIfc::Read()
{
    String in;
    float r[3]; // for data (consider moving to data case)
    char buf[128] ={0}; // dor incoming msg
    ClientData data; // return value

    Debug::msg(("Reading..."));

    in = port->readStringUntil(endch); // will wait for endchar
    in.toCharArray(buf, 128); 

    //readuntil(buf,&endch);
    //in +=buf;
    //Serial.print(F("Read following: "));

    //Serial.println(buf);
    delay(5);
    data.raw = in;

    //Serial.print("ClientIfc:ReadFromClient>" + in + "<");  -- Dont work, WHY?
    Debug::msg("ClientIfc:ReadFromClient> ");
    Debug::msg(buf);

    char first = buf[0]; // The convention: first char indicates type of message
    if (first == commch)
    {
        data.type = "command";
        data.command = in.substring(1);
    }
    else if (first == msgch)
    {
        data.type = "msg";
        data.msg = in.substring(1);
    }
    else if (first == datach)
    {
        data.type = "data";
        ParseData(buf, r); // parse data from buffer to floar array r
        data.data.setVals(r[0], r[1], r[2]); // assign parsed to return value
        Debug::msg("Read Data:");
        char tmp[20];
        data.data.toChar(tmp);
        Debug::msg(tmp);
    }
    else
    { // if first char is something strange
        Serial.print("ClientIfc:Error>");
        Serial.print(buf);
        Serial.print("\n");
    }
    return data;
}

ClientIfc::Send(char c)
{
    port->write(c);
}
ClientIfc::SendCommand(String in)
{
    String p = "";
    p += commch + in + endch; 
    port->print(p);
}
ClientIfc::SendMessage(String in)
{
    String p = "";
    p += msgch + in + endch;
    port->print(p);
}
ClientIfc::SendData(SensorData *out)
{   // We will form a char array here using convention
    char str[28] = {0};
    char val[8] = {0};
        dtostrf(out->x,3,2,val);
    sprintf(str,"-%s;",val); // '-' is where startchar will be 
        dtostrf(out->y,3,2,val);
    sprintf(str,"%s%s;",str,val);
      dtostrf(out->z,3,2,val);
    byte i =sprintf(str,"%s%s;-",str,val); // i is length of final array
    str[0]=  this->datach;
    str[i-1] = this->endch;  // setting endchar 

     Debug::msg("Sending data...");
     Debug::msg(str);

    port->print(str); // Send data to port
}
ClientIfc::ParseData(char * ch, float r[])
{
    unsigned short k =   strlen(ch);
    unsigned short j;
    unsigned short n;
    char tmp[16];
    j = 1;
    n = 0;

    ch[k - 1] = endch;
    for (byte i = 1; i < k; i++)
    {
        if ((ch[i] == ';') || (ch[i] == endch)) // The convention: every number ends with ';'
        {
            short p  =0;
            for (short l = j;l<i-1;l++){
                if(p<5){
                tmp[p++] = ch[l];
                }
            }
           // in = String(substr(ch, j, i - 1));
           float tf = (float)atof(tmp);
            r[n] = (tf < 50 ) ? tf : 0.0f; // An attempt to filter improperly parsed data
            // TODO: implement parsing error detection, maybe send some additional data to drone

            // Serial.println(r[n]);
            // Serial.print(j); Serial.print(i);
            // Serial.println(substr(ch,j, i - 1));
            // delay(3);
            n++;
            j = i + 1; // set start of next number to next char
        }
    }
}
String ClientIfc::substr(char *in, int s, int e)
{ // It wil work only here without sizes
    String r = "";
    for (byte i = s; i < e; i++)
    {
        r += in[i];
    }
    return r;
}
ClientIfc::readuntil(char *buf,char *en)
{
    Serial.println(F("Reading... "));
    short i;
    bool del = true;

    while (true)
    {
        if (port->available())
        {
            /*
            if (del)
            { // wait a little if firstly avail;
                delay(2);
                del = false;
            }*/
            buf[i] = port->read();
            if (buf[i++] == *en)
                break;
        }
        else
            del = true;
    }
    Serial.print(F("\n END Reading.. "));
}
