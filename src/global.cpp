#include "global.h" 

#include <EEPROM.h>


Alarm alarm;
tmElements_t tm;
bool doDisplay=true;
uint8_t rate=1;
uint8_t sunriseType=1;
uint8_t scheme1=1,scheme2=2;
uint8_t trainState[12];
bool voiceReco;


void loadFromEEPROM(){

  EEPROM.get(0,alarm.Hour);    // 
  EEPROM.get(1,alarm.Minute);    // 
  EEPROM.get(2,alarm.on);    // 
  EEPROM.get(3,scheme1);    // 
  EEPROM.get(4,sunriseType);    // 
  EEPROM.get(5,rate);    // 
  for (int i=0;i<12;i++) {
    EEPROM.get(6+i,trainState[i]);    //   
  }
  EEPROM.get(18,scheme2);    // 
  
}

void saveToEEPROM() {

  EEPROM.put(0,alarm.Hour);    // 
  EEPROM.put(1,alarm.Minute);    // 
  EEPROM.put(2,alarm.on);    // 
  EEPROM.put(3,scheme1);    // 
  EEPROM.put(4,sunriseType);    // 
  EEPROM.put(5,rate);    // 
  for (int i=0;i<12;i++) {
    EEPROM.put(6+i,trainState[i]);    //   
  }
  EEPROM.put(18,scheme2);
}

