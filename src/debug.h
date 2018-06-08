#define DEBUGLN(x) Serial.println(x)
#define DEBUG(x) Serial.print(x);Serial.print(" ");



#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

//#include <stdarg.h>

void printCmd(int i);
//
//void p(char *fmt, ... );
//void p(const __FlashStringHelper *fmt, ... );
