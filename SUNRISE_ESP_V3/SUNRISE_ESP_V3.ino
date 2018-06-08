#include <math.h>
#include "wifi.h"
#include "eeprom.h"


void setup() {

  delay(1000);

  Serial.begin(74880);

  Serial.println();

  Serial.print("Configuring access point...");
  eeprom_setup();
  wifi_setup();

}


String find(String req,String param){
//  String req=String(reqB);
  int start=req.indexOf(param+"=");
  start=req.indexOf("=",start);
  if (start<0) return "";
  start++;
  int jj=req.indexOf("&",start);
  int kk=req.indexOf(" ",start);
  int end=jj;
  
  if ( jj < 0  ||  (kk >0 && kk <jj) ) end=kk;
  
  String val=req.substring(start,end);

  return val;
}


void handleCmd(String req) {
  String str=find(req,"ssid");
  str.toCharArray(esid,MAX_SSID);
  Serial.print("SSID=");Serial.println(esid);
  str=find(req,"password");
  str.toCharArray(epass,MAX_PASS);
  Serial.println("PASSWORD=");Serial.print(epass);
  eeprom_write(); 
}


void loop() {

 
  // Wait until the client sends some data
 
  String  request = wifi_getCommand();
                    

  if (request != "") {
    handleCmd(request);
 
    delay(1);

    // wifi_response("OK");
    Serial.println("Client disconnected");
    Serial.println("");
  }

}


