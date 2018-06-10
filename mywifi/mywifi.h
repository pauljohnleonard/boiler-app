#ifndef _MYWIFI_H
#define _MYWIFI_H

#include <Arduino.h>
#include <WiFiClient.h>

class Request
{
  public:

    static Request emptyRequest; 
    WiFiClient client;

    String req;
    String type;
    String path;
    Request(String &str);
    Request();
    String getParam(String param);
    void setup(String str);
    void print();
    operator bool() const;
   
};

// Interface to handle incoming messages.
class MyHandler
{

  public:
    virtual String handleRequest(Request &req) = 0;
};

class MyWifi
{


    WiFiClient client;
    Request request = Request();
    Request emptyRequest = Request();
    
  public:
    static const int UNDEF = -1;
    static const int STATION = 1;
    static const int AP = 2;

    int state = UNDEF;

  public:
    MyWifi();
    void setupStation(String &network, String &password, String &domain);
    void setupStaticStation(int ip1, int ip2, int ip3, int ip4, String &network, String &password);
    void setupAP(String &apssid, String &domainAP);
    void loop(MyHandler &h);


  private:
    Request getRequest();
    void response(String &response);
};

#endif
