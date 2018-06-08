#ifndef _EEPROM_H
#define _EEPROM_H

#include <Arduino.h>
#include <EEPROM.h>

extern String esid;
extern String epass;

void eeprom_setup();
void eeprom_read();
void eeprom_write();


#endif

