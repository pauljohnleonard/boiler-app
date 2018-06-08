#include <CapacitiveSensor.h>
#include "debug.h"
#include "ui.h"
#include "global.h"
#include "pins.h"
#include "display.h"
#include "vr.h"

// UI
#define MIN_PRESS_TIME   50
#define PRESS_TIMEOUT  2000
#define DEBOUNCE_TIME    40
#define LONG_PRESS     1000
#define WAIT_FOR_PRESS 2000
bool buts[12];

// capacitor sensor
#define CAP_SAMPLES  4
#define CAP_THRESH  20

CapacitiveSensor cs[12] = { CapacitiveSensor(CAP_SEND, CAP_0),
                            CapacitiveSensor(CAP_SEND, CAP_1),
                            CapacitiveSensor(CAP_SEND, CAP_2),
                            CapacitiveSensor(CAP_SEND, CAP_3),
                            CapacitiveSensor(CAP_SEND, CAP_4),
                            CapacitiveSensor(CAP_SEND, CAP_5),
                            CapacitiveSensor(CAP_SEND, CAP_6),
                            CapacitiveSensor(CAP_SEND, CAP_7),
                            CapacitiveSensor(CAP_SEND, CAP_8),
                            CapacitiveSensor(CAP_SEND, CAP_9),
                            CapacitiveSensor(CAP_SEND, CAP_10),
                            CapacitiveSensor(CAP_SEND, CAP_11)
                          };

void setupUI() {

  for (int i = 0; i < 12; i++) {
    cs[i].set_CS_AutocaL_Millis(0xFFFFFFFF);
    cs[i].set_CS_AutocaL_Millis(0x100);
    cs[i].set_CS_Timeout_Millis(10);
  }

}


int checkButtons() {
  int cnt = 0;
  for (int i = 0; i < 12; i++) {
    int tt = cs[i].capacitiveSensor(CAP_SAMPLES);
    //DEBUG(tt);
    if (tt > CAP_THRESH ) {
      buts[i] = true;
      cnt++;
  //    delay(50);
    } else {
      buts[i] = false;
    }
  }

  if (cnt > 0) {
  for (int i = 0; i < 12; i++) {
    DEBUG(buts[i])
  } 
  DEBUG(" : ");
  DEBUG(cnt);
  DEBUGLN("");
  }
  return cnt;
}

void waitForRelease() {
  while (checkButtons()) {
  };  // Wait for release
  delay(10);  // debounce
}


int whichBut() {
  if (checkButtons()) {
    for (int i = 0 ; i < 12; i++) {
      if (buts[i])  return i;
    }
  }
  return -1;
}

int waitForButton(unsigned long tout) {

  unsigned long tEnd = millis() + tout;

  int key = -1;
  while (millis() < tEnd) {
    key = whichBut();
    if (key > 0) return key;
  }

  return -1;
}

void butCommand(int key1) {

  DEBUG(" butCOMMAND "); DEBUGLN(RATE); DEBUGLN(key1);
  int key2;
  switch (key1) {
    case ALARM:
      alarm.on = !alarm.on;
      saveToEEPROM();
      return;
    case SUNRISE:
      displayPrompt();
      waitForRelease();
      key2 = waitForButton(WAIT_FOR_PRESS);
      DEBUG("ST "); DEBUGLN(key2);

      if (key2 >= 1) {
        sunriseType = key2;
        saveToEEPROM();
      }
      loadTop(false);
      waitForRelease();
      doAlarm();
      return;

    case OFF1:
    case OFF2:
      doDisplay = ! doDisplay;
      return;

    case SCHEME:
      displayPrompt();
      waitForRelease();
      key2 = waitForButton(WAIT_FOR_PRESS);
      if (key2 >= 1) {
        scheme1 = min(7,key2);
      }
      displayPrompt({0,255,0},bit2col(scheme1));
   
      waitForRelease();
      key2 = waitForButton(WAIT_FOR_PRESS);
      if (key2 >= 1) {
        scheme2 = min(7,key2);
      }
      displayPrompt({0,255,0},bit2col(scheme1),bit2col(scheme2));
      delay(500);
      displaySetScheme();
      
      saveToEEPROM();
      
      
      
      return;

    case RATE:
      displayPrompt();
      waitForRelease();
      key2 = waitForButton(WAIT_FOR_PRESS);

      if (key2 >= 1) {
        rate = min(7,key2);
        saveToEEPROM();
      }
      loadTop(false);
      waitForRelease();
      doAlarm();
      return;
  }

}



/// entry point into this system
void ui() {
  int state, state3, substate;
  int pressed[5];
  unsigned long tPress = 0, tRelease = 0;
  state3 = 0;
  int n;
  bool done = false;
  int longPress0 = 0;


  if ( (millis() - tRelease) < PRESS_TIMEOUT) return;   // Give time for user to release


  while (!done) {
    state = state3 / 3;
    substate = state3 % 3;
    n = checkButtons();

    if ( n && !doDisplay) {
      doDisplay = true;
      displayAutoBright();
      return;
    }

    if (n > 2) {
      DEBUG(" N = "); DEBUGLN(n);
    }

    if (n == 2) {
      if (buts[0]) {
        DEBUGLN(" BUT0 N=2" );
        for (int i = 6 ; i < 12; i++) {
          if (buts[i])  {
            DEBUGLN(i);
            butCommand(i);
            break;
          }
        }
      } else if (buts[9]) {
        if (buts[1]) {
          resetVR();
          trainVR();
          return;
        } else if (buts[2]) {
          trainVR();
          return;
        } else if (buts[3]) {   
            while(checkButtons() && buts[9]) {
                  voiceReco = !voiceReco;
                  displayRecoState();
                  displayShow();           
                  while(checkButtons() && buts[3]);
                  while(checkButtons() && !buts[3] && buts[9]);
            }
        }
      }
      return;
    }

    switch ( substate) {    // 0 waiting   1 - pressed  2- releasing

      case 0: // waiting for a press
        if ( n == 0 ) {
          if ( state == 0 ) {  // nothing pressed abandon routine
            return;
          } else if ((millis() - tRelease) > PRESS_TIMEOUT)   {
            // Serial.println(" Time out in UI ");
            return;
          }
        }

        if ( (millis() - tRelease) < MIN_PRESS_TIME) break;     //  Wait for a bit after release

        for (int i = 0 ; i < 12; i++) {
          if (buts[i]) {
            pressed[state] = i;
            tPress = millis();
            if (state == 0) longPress0 = 0;
            state3 += 1;
            break;
          }
        }
        break;

      case 1:   // was pressed

        //        if (state == 0) {
        //          if (n == 2) {
        //            alarm.on = !alarm.on;
        //            display();
        //            waitForRelease();
        //            return;
        //          } else if ( n == 3)  {
        //            dispSunrise(0);
        //            waitForRelease();
        //            doAlarm();
        //            return;
        //          }
        //        }

        if ( (millis() - tPress) < DEBOUNCE_TIME) break;

        if (state == 0 ) {
          longPress0 =  (millis() - tPress) / LONG_PRESS;
          //   Serial.println(longPress0);
        }

        displayClear();

        if (state3 == 1) {


          if (longPress0 == 0) {
            displayAlarmHour(pressed[0]);
          } else if (longPress0 == 1) {
            tm.Hour = pressed[0];
            displayTimeHour(pressed[0]);
          } else if (longPress0 == 2) {
            tm.Hour = pressed[0] + 12;
            displayPM(true);
            displayTimeHour(pressed[0]);
          }


        } else if (state3 == 4)  {

          if (longPress0 > 0) {
            displayTimeHour(pressed[0]);
            displayTimeMinute(pressed[1]);
          } else {
            displayAlarmHour(pressed[0]);
            displayAlarmMinute(pressed[1]);
          }
        }

        displayShow();

        if ( ! buts[pressed[state]]  ) {

          if (  (millis() - tPress) < MIN_PRESS_TIME) { // Throw away glitches
            state3 -= 1;
            break;
          }

          tRelease = millis();
          if (state == 1) done = true;
          state3 += 1;

        }
        break;

      case 2:    // debounce the release
        if ( buts[pressed[state]] ) { // glitch ?
          state3 -= 1;
          break;
        }
        if ( (millis() - tRelease) > DEBOUNCE_TIME) {
          state3 += 1;
          if (state3 == 6) {
            done = true;
            break;
          }
        }
        break;
    }
  }

  if (longPress0 > 0 ) {
    tm.Minute = pressed[1] * 5;
    RTC.write(tm);
  } else {
    alarm.Hour = pressed[0];
    alarm.Minute = pressed[1] * 5;
    alarm.on = true;
    saveToEEPROM();

  }
}

