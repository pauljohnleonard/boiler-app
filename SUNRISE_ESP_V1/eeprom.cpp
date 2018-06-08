#include "eeprom.h"
 
String esid;
String epass;

void eeprom_setup() {
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
  String esid;
  for (int i = 0; i < 32; ++i)
  {
    esid += char(EEPROM.read(i));
  }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
  {
    epass += char(EEPROM.read(i));
  }
  Serial.print("PASS: ");
  Serial.println(epass);
}

void eeprom_write() {
  Serial.println("clearing eeprom");
  for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
  }
  Serial.println(esid);
  Serial.println("");
  Serial.println(epass);
  Serial.println("");

  Serial.println("writing eeprom ssid:");
  for (unsigned int i = 0; i < esid.length(); ++i)
  {
    EEPROM.write(i, esid[i]);
    Serial.print("Wrote: ");
    Serial.println(esid[i]);
  }
  Serial.println("writing eeprom pass:");
  for (unsigned int i = 0; i < epass.length(); ++i)
  {
    EEPROM.write(32 + i, epass[i]);
    Serial.print("Wrote: ");
    Serial.println(epass[i]);
  }
  EEPROM.commit();
}
