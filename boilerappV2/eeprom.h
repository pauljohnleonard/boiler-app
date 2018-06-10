#ifndef _EEPROM_H
#define _EEPROM_H

#include <Arduino.h>
#include <EEPROM.h>


 #define MAX_SSID 32
 #define MAX_PASS 64
 #define MAX_TOT (MAX_SSID+MAX_PASS)
 
extern char esid[MAX_SSID];
extern char epass[MAX_PASS];

void eeprom_setup();
void eeprom_read();
void eeprom_write();


#endif

