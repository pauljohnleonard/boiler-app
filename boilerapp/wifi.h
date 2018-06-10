#ifndef _WIFI_H
#define _WIFI_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// #include <ESP8266WebServer.h>


extern WiFiServer server; //Service Port

class MyWifi {
  public:
    static const int UNDEF = -1;
    static const int STATION  = 1;
    static const int AP = 2;


    int state=UNDEF;

  public:
    void setup();
     void setupStatic();
    void setupAP();
    String getCommand();
    void handleCommand(String request);
    void response(String response);

};

#endif
