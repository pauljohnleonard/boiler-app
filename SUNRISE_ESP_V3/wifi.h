#ifndef _WIFI_H
#define _WIFI_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// #include <ESP8266WebServer.h>


extern WiFiServer server; //Service Port

#define wifi_UNDEF    -1
#define wifi_STATION   1
#define wifi_AP  2

extern int state;

void wifi_setup();
String wifi_getCommand();
void wifi_response(String resp);

#endif
