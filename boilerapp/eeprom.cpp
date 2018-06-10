#include "eeprom.h"

 
char  esid[MAX_SSID];
char  epass[MAX_PASS];

void eeprom_setup() {
  Serial.print("Setup EEPROM ..."); 
  EEPROM.begin(512);
  delay(10);
  eeprom_read();
}


void eeprom_read() {
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  for (int i = 0; i < MAX_SSID; ++i)
  {
    esid[i] = EEPROM.read(i);
  }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  for (int i = 0 ; i < (MAX_PASS) ; ++i)
  {
    epass[i]= EEPROM.read(MAX_SSID+i);
  }
  Serial.print("PASS: ");
  Serial.println(epass);
}

void eeprom_write() {
  Serial.println("clearing eeprom");
  for (int i = 0; i < MAX_TOT; ++i) {
    EEPROM.write(i, 0);
  }
  
Serial.print("writing eeprom ssid:");
  
  Serial.println(esid);

  for (unsigned int i = 0; i < MAX_SSID; ++i)
  {
    EEPROM.write(i, esid[i]);
  }

  Serial.print("writing eeprom pass:");
  Serial.println(epass);
  
  for (unsigned int i = 0; i < MAX_PASS; ++i)
  {
    EEPROM.write(MAX_SSID + i, epass[i]);
  }
  
  EEPROM.commit();
}
