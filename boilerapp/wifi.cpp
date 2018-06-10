#include "eeprom.h"
#include "wifi.h"
#include "util.h"

Util util = Util();

const char *apssid = "SunriseSetup";

WiFiServer server(80);//Service Port


void MyWifi::setup() {

  Serial.print(esid);
  Serial.print(" Password:");
  Serial.println(epass);

  delay(500);
  
  for (int j = 0; j < 200; j++) {
    WiFi.begin("virgin-2G", "slugsarenice");
    Serial.print(" Setup atempting to connect to network ");

    for (int i = 0; i < 10; i++) {
      if (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      } else {
        this->state = MyWifi::STATION;

        Serial.println("");
        Serial.println("WiFi connected");

        // Start the server
        server.begin();
        Serial.println("Server started");

        // Print the IP address
        Serial.print("Use this URL to connect: ");
        Serial.print("http://");
        Serial.print(WiFi.localIP());
        Serial.println("/");
        return;
      }
    }
    Serial.println(" Failed to connect to network");

    WiFi.disconnect();
  }



}


void MyWifi::setupStatic() {

  Serial.print(esid);
  Serial.print(" Password:");
  Serial.println(epass);

// Static IP details...
IPAddress ip(192, 168, 0, 222);
IPAddress gateway(192, 168, 0, 249);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 0, 249);

// Static IP Setup Info Here...
WiFi.config(ip, dns, gateway, subnet); //If you need Internet Access You should Add DNS also...
  // WiFi.begin(ssid, password);

  delay(500);

  
  for (int j = 0; j < 200; j++) {
    WiFi.begin("virgin-2G", "slugsarenice");
    Serial.print(" Setup atempting to connect to network ");

    for (int i = 0; i < 10; i++) {
      if (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      } else {
        this->state = MyWifi::STATION;

        Serial.println("");
        Serial.println("WiFi connected");

        // Start the server
        server.begin();
        Serial.println("Server started");

        // Print the IP address
        Serial.print("Use this URL to connect: ");
        Serial.print("http://");
        Serial.print(WiFi.localIP());
        Serial.println("/");
        return;
      }
    }
    Serial.println(" Failed to connect to network");

    WiFi.disconnect();
  }



}







void MyWifi::setupAP() {

  Serial.print("Setting up access point on : ");
  Serial.println(apssid);

  WiFi.softAP(apssid);
  //
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }

  Serial.println("");
  Serial.println("WiFi connected");

  this->state = MyWifi::AP;

  // Start the server
  server.begin();
  Serial.println("Server started");

  IPAddress myIP = WiFi.softAPIP();

  Serial.print("Device IP address: ");
  Serial.println(myIP);


}

WiFiClient client;

String MyWifi::getCommand() {

  client = server.available();


  if (!client) return "";

  Serial.println(" Client connected");

  bool avail = 0;
  for (int i = 0; i < 200; i++) {
    avail = client.available();
    if (avail) break;
    delay(1);
  }

  if (avail) {
    Serial.print(" avaible: "); Serial.println(client.available());

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.print(" requst: "); Serial.println(request);

    client.flush();
    return request;

    return request;
  } else {

    client.stop();
    return "";
  }
}


void MyWifi::response(String mess) {

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Access-Control-Allow-Origin: *");
  client.println(""); //  do not forget this one
  client.println(mess);
  client.stop();

}




void MyWifi::handleCommand(String req) {
  String cmd = util.find(req, "cmd");

  switch (this->state) {
    case MyWifi::STATION:
      Serial.print("CMD :");
      Serial.println(req);

      if (cmd == "ping") {
        MyWifi::response("pong");
        return;
      }
      break;

    case MyWifi::AP:

      if (cmd == "setup") {
        String str = util.find(req, "ssid");
        str.toCharArray(esid, MAX_SSID);
        Serial.print("SSID="); Serial.println(esid);
        str = util.find(req, "password");
        str.toCharArray(epass, MAX_PASS);
        Serial.println("PASSWORD="); Serial.print(epass);
        eeprom_write();

        MyWifi::setup();

        MyWifi::response("STATION");
        return;

      }

  }
  MyWifi::response(" ? uknown command :" + req);

}

