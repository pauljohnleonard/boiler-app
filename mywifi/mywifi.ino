#include <math.h>
#include <EEPROM.h>
#include <Arduino.h>
#include "mywifi.h"

#define MAX_SSID 32
#define MAX_PASS 64

#define MAX_BUFF  _max(MAX_SSID,MAX_PASS)
#define EEPROM_SIZE (MAX_SSID+MAX_PASS)

void eeprom_setup();
void eeprom_read();
void eeprom_write();


String apssid = "boilerAP";
String domainAP ="light";


String domainStation ="boiler";

String network = "";
String password = "";

MyWifi wifi = MyWifi();

void eeprom_setup() {
  Serial.print("Setup EEPROM ...");
  EEPROM.begin(EEPROM_SIZE);
  delay(10);
}

String eeprom_read(int start,unsigned int maxchr) {
  delay(10);
  char buff[MAX_BUFF];
   
  for (unsigned int i = 0; i < maxchr; ++i)
  {
    buff[i] = EEPROM.read(start+i);
    if (buff[i]==0) break;
  }
  String str = buff;
  return str;
}

void eeprom_write(int start,String str,unsigned int maxchr) {
 
  for (unsigned int i = 0; i < _min(maxchr,str.length()); ++i)
  {
    EEPROM.write(i+start, str.charAt(i));
  }

  EEPROM.commit();
}



void setup() {

  delay(10);
  
  eeprom_setup();
  
  network=eeprom_read(0,MAX_SSID);
  password=eeprom_read(MAX_SSID,MAX_PASS);
  
  
  Serial.begin(74880);
  Serial.println("Boiler App");
  Serial.print(F(__DATE__));
  Serial.print("  ");
  Serial.println(F(__TIME__));


  // wifi.setupStatic(192, 168, 0, 222,network,password);
  wifi.setupStation(network,password,domainStation);
  if (wifi.state < 0) {  //

    Serial.println(" Unable to connect to network setting up access point.");
    wifi.setupAP(apssid,domainAP);
  }

}



class MyHandlerAP: public MyHandler  {


    String handleRequest(Request &req) {


      if (req.path == "setup") {
        String ssid = req.getParam("ssid");
        Serial.print("SSID="); Serial.println(ssid);

        
        String pass = req.getParam("password");
        Serial.println("PASSWORD="); Serial.print(pass);
        
        eeprom_write(0,network,MAX_SSID);
        eeprom_write(MAX_SSID,password,MAX_PASS);
        wifi.setupStation(network,password,domainStation);

        return "STATION";

      }

      return " ? uknown command :" + req;

    }
};


class MyHandlerStation: public MyHandler  {

    String handleRequest(Request &req) {

      if (req.path == "ping") {
        return "pong";
      }

      return " ? uknown command :" + req.path;
    }

};



MyHandlerAP myHandlerSTATION;
MyHandlerStation myHandlerAP;


void loop() {


  switch (wifi.state)  {

    case MyWifi::AP:
      wifi.loop(myHandlerAP);
      break;

    case MyWifi::STATION:
      wifi.loop(myHandlerSTATION);
      break;


    case MyWifi::UNDEF:
      wifi.setupAP(apssid,domainAP);
      break;
  }

}


