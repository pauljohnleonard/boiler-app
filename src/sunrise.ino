#include "vr.h"
#include "global.h"
#include "ui.h"
#include "display.h"


//-------------------------- SETUP AND LOOP CODE ---------------------------

void setup() { 
  Serial.begin(9600);
  while(!Serial);
  Serial.println(F(__FILE__));
  
  setupDisplay();
  setupUI();
  setupVR();
  loadFromEEPROM();
  displaySetScheme();

}

void loop() {
  visitVR();
  if (alarm.on) {
    if ( (alarm.Hour == tm.Hour) && (alarm.Minute == tm.Minute) && (tm.Second == 0 )) {
      doAlarm();
    }
  }
  ui();
  RTC.read(tm);
  display();
  waitForRelease();  // debounce
}
