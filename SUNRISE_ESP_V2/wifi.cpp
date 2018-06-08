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



  WiFiClient client = server.available();

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


    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Access-Control-Allow-Origin: *");
    client.println(""); //  do not forget this one
    client.println("OK");
    client.stop();
    return request;
  } else {
     client.flush();


    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Access-Control-Allow-Origin: *");
    client.println(""); //  do not forget this one
    client.println("ERRUM");
    client.stop();
    client.stop();

  }
}



