#include "eeprom.h"
#include "wifi.h"


const char *apssid = "SunriseSetup";


WiFiServer server(80);//Service Port



void wifi_setup() {

  WiFi.softAP(apssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");

  Serial.println(myIP);
}


String wifi_getCommand() {

 
  
  WiFiClient client=server.available();

  if (!client) return "";
  
  Serial.println(" WAitng for client");
  
  while (!client.available()) {
    delay(5);
  }
  
  Serial.println(" done waiting ");
  
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  // Serial.println(request);
  
  client.flush();

  
   // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Access-Control-Allow-Origin: *");
  client.println(""); //  do not forget this one
  client.println(request);
  
  return request;
  
 }



