#ifndef _WIFI_H
#define _WIFI_H

#include <ESP8266WiFi.h>

#include <WiFiClient.h>

// #include <ESP8266WebServer.h>


extern WiFiServer server; //Service Port

void wifi_setup();
String wifi_getCommand();
void wifi_response(String resp);

#endif
