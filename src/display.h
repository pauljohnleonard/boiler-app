#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <Adafruit_NeoPixel.h>
#include "vr.h"


typedef struct {uint8_t r,g,b;} MyColor;

void setupDisplay();
void display();
void dispSunrise(unsigned long t);
void doAlarm();
void displayClear();
void displayTimeHour(int hr);
void displayTimeMinute(int hr);
void displayAlarmHour(int hr);
void displayAlarmMinute(int hr);
void displayPM(bool pm);
void displayVRTrain(int key,TrainState state);
void displayVRTrained(uint8_t trainState[] );
void setRadPixels(uint8_t min, uint32_t c, uint8_t flags);
void displayShow();
void displaySetScheme();
void displaySetSchemeVR(int key);
void displayAutoBright();
void displayRecoState();
void displayPrompt(MyColor c0={0,255,0},MyColor c1={0,0,0},MyColor c2={0,0 ,0});
MyColor bit2col(int key);

#endif
