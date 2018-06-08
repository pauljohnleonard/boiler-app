#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <Time.h>
#include <TimeLib.h>



#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define DEBUGING 1

#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

typedef struct {
  uint8_t Hour;
  uint8_t Minute;
  bool on;
} Alarm;

extern Alarm alarm;
extern tmElements_t tm;
extern bool voiceReco;
extern bool doDisplay;
extern uint8_t rate;
extern uint8_t sunriseType;
extern uint8_t scheme1,scheme2;
extern uint8_t trainState[12];

void loadFromEEPROM();
void saveToEEPROM();


#endif
