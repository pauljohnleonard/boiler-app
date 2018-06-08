#include "debug.h"


void printCmd(int i) {

  switch(i){
    case 0:   Serial.println(F("scheme"));return;
    case 1 :Serial.println(F("1"));return;
    case 2 :Serial.println(F("2"));return; 
    case 3 :Serial.println(F("3"));return; 
    case 4 :Serial.println(F("4"));return; 
    case 5 :Serial.println(F("5"));return;
    case 6 :Serial.println(F("6"));return;
    case 7 :Serial.println(F("alarm"));return; 
    case 8 :Serial.println(F("display"));return; 
    case 9 :Serial.println(F("off"));return; 
    case 10 :Serial.println(F("weekend")); return; 
    case 11 :Serial.println(F("sunrise")); return;
  }
}


//
//#include <stdarg.h>
//
//
//void p(char *fmt, ... ){
//        char buf[128]; // resulting string limited to 128 chars
//        va_list args;
//        va_start (args, fmt );
//        vsnprintf(buf, 128, fmt, args);
//        va_end (args);
//        Serial.print(buf);
//}
//
//void p(const __FlashStringHelper *fmt, ... ){
//  char buf[128]; // resulting string limited to 128 chars
//  va_list args;
//  va_start (args, fmt);
//#ifdef __AVR__
//  vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
//#else
//  vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
//#endif
//  va_end(args);
//  Serial.print(buf);
//}

