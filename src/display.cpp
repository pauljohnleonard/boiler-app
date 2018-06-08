
#include "display.h"
#include "pins.h"
#include "global.h"
#include "ui.h"
#include "vr.h"
#include "util.h"
#include "debug.h"
#include "global.h"


// CLOCK DISP
#define ALARM_MINUTE_PIX  0x3
#define ALARM_HOUR_PIX  0x1
#define TIME_HOUR_PIX  0x3
#define TIME_MINUTE_PIX 0x7
#define TIME_SECOND_PIX 0x4
#define ALARM_FADE_IN_TIME 60000     // 1 minute    RATE*RATE

// LIGHT DIMMER

#define HYST  0.1
#define NLEDS 37
#define LOW_VOLT_MILLIS  3300

// Light map
#define N_LT 8

float lt[N_LT][2] = { { -0.0001, 1}, {20, 4}, {100, 6}, {200, 8}, {300, 9} , {500, 10},  {800, 11},  {1024, 20}};

// --- GLOBALS --------------

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NLEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

// Colours
MyColor  BACK_COL = {0, 0, 0};
MyColor  TIME_COL = {0, 0, 255};
MyColor  VR_COL1 = {255, 0, 0};
MyColor  ALARM_COL = {0, 255, 0};


// VR Colours
MyColor  VR_TRAINED_COL = {0, 255, 0};
MyColor  VR_UNTRAINED_COL = {255, 255, 0};
MyColor  VR_ERROR_COL = {255, 0, 0};
MyColor  VR_UNKNOWN_COL = {255, 255, 0};


MyColor VR_SUCCESS_COL = {0, 0, 255};
MyColor VR_WAIT_COL = {255, 0, 0};
MyColor VR_SPEAK_COL = {0, 255, 0};
MyColor VR_NOVOICE_COL = {255, 255, 0};
MyColor VR_AGAIN_COL = {255, 0, 255};
MyColor VR_NOMATCH_COL = {255, 0, 0};

float brightness = 1.0;
unsigned long alarm_fade_time;

void setBrightness(float b) {
  // Serial.println(brightness);
  brightness = max(0, min(1.0, b));

}

uint32_t doColor(MyColor c) {

  uint8_t r = c.r * brightness;
  uint8_t g = c.g * brightness;
  uint8_t b = c.b * brightness;
  return strip.Color(r, g, b);

}


void displayRecoState() {
  displayClear();
  if (voiceReco) {
    strip.setPixelColor(0, doColor({0, 255, 0}));
  } else {
    strip.setPixelColor(0, doColor({255, 0, 0}));
  }
  displayShow();
  delay(500);
  return;
}

float filter(float  x) {
  static float valF = 1.0, val = 1.0;
  valF = valF * 0.99 + 0.01 * x;
  if ((val - valF) > HYST) {
    val = valF + HYST;
  } else if ((valF - val) > HYST) {
    val = valF - HYST;
  }
  return val;
}

void setClockPixel(uint8_t hour, uint8_t rad, MyColor c) {

  if (rad == 0) {
    strip.setPixelColor(0, doColor(c));
  } else {
    int pix = ((28 - hour) % 12) * 3;
    if (hour % 2) {
      pix += 4 - rad;
    } else {
      pix += rad;
    }
    strip.setPixelColor(pix, doColor(c));
  }
}

void setRadPixels(uint8_t min, MyColor c, uint8_t flags) {
  if (flags & 0x1) setClockPixel(min, 1, c);
  if (flags & 0x2) setClockPixel(min, 2, c);
  if (flags & 0x4) setClockPixel(min, 3, c);
}

void displayClear() {

  strip.clear();
}

float mapLight(float raw) {
  for (int i = 1 ; i < N_LT ; i++) {
    if ( raw > lt[i - 1][0] && raw <= lt[i][0]) {
      return (     lt[i - 1][1] * (lt[i][0] - raw) + lt[i][1] * (raw - lt[i - 1][0])  ) / (lt[i][0] - lt[i - 1][0]);
    }
  }
  return 0;
}

void drawTime() {
  setRadPixels(tm.Second / 5, TIME_COL, 0x4);
  setRadPixels(tm.Minute / 5, TIME_COL, 0x7);
  setRadPixels(tm.Hour % 12, TIME_COL, 0x3);
  if (tm.Hour / 12) {
    strip.setPixelColor(0, doColor(TIME_COL));
  } else {
    strip.setPixelColor(0, doColor(BACK_COL));
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
MyColor Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return {255 - WheelPos * 3, 0, WheelPos * 3};
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return {0, WheelPos * 3, 255 - WheelPos * 3};
  }
  WheelPos -= 170;
  return {WheelPos * 3, 255 - WheelPos * 3, 0};
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(unsigned long t) {
  uint16_t i, j = t / 10;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, doColor(Wheel(((i * 256 / strip.numPixels()) + j) & 255)));
  }
}

#define DIFF  20
void rainbowCycleRad(unsigned long t) {

  t = t / 10;

  float t1 = t / 10000.0;
  if (t1 > 1.) t1 = 1.;
  uint8_t r = t1 * 255;
  uint8_t g = t1 * t1 * 250;
  uint8_t b = t1 * t1 * t1 * 230;
  MyColor c3 = Wheel( (t) & 255 );
  MyColor c2 = Wheel( (t + DIFF) & 255 );
  MyColor c1 = Wheel( (t + DIFF * 2) & 255 );
  MyColor c0 = Wheel( (t + DIFF * 3)  & 255 );

  setClockPixel(0, 0, c0);

  for (int i = 0; i < 12; i++) {
    setClockPixel(i, 1, c1);
    setClockPixel(i, 2, c2);
    setClockPixel(i, 3, c3);
  }

}


// Fill the dots one after the other with a color
void setAll(MyColor c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, doColor(c));
  }
}



uint8_t *t2rgb(long t) {
  static uint8_t rgb[3];
  float t2 = t / float(alarm_fade_time);
  t2 = min(1.0, t2);

  rgb[0] = 255;
  rgb[1] = 255 * t2;
  rgb[2] = 255 * t2 * t2;
  return rgb;
}


void t2rgb1(long t, MyColor &rgb) {

  // Serial.println(t);
  t = max(0, t);
  float t2 = t / float(alarm_fade_time);
  float t3 = max(0, min(1.0, (t / float(alarm_fade_time) - 0.8)));

  t2 = min(1.0, t2);

  rgb.r = 255 * t2;
  rgb.g = 255 * t2 * t2;
  rgb.b = 255 * t3;

}



void vanilla(unsigned long t) {
  uint8_t *rgb = t2rgb(t);
  setAll({rgb[0], rgb[1], rgb[2]});
}


void hannah(unsigned long t) {
  MyColor rgb;
  t2rgb1(t, rgb);
  setAll(rgb);
}


void hannah2(unsigned long t) {

  MyColor c0;
  t2rgb1(t, c0);
  MyColor c1;
  t2rgb1(t - 0.1 * alarm_fade_time, c1);
  MyColor c2;
  t2rgb1(t - 0.2 * alarm_fade_time, c2);
  MyColor c3;
  t2rgb1(t - 0.3 * alarm_fade_time, c3);

  setClockPixel(0, 0, c0);

  for (int i = 0; i < 12; i++) {
    setClockPixel(i, 1, c1);
    setClockPixel(i, 2, c2);
    setClockPixel(i, 3, c3);
  }

}

void displayPrompt(MyColor c0, MyColor c1, MyColor c2) {

  displayClear();
  setClockPixel(0, 0, c0);
  for (int i = 0; i < 12; i++) {
    setClockPixel(i, 1, c1);
    setClockPixel(i, 2, c2);
  }
  displayShow();
}


MyColor randomish(MyColor &c) {
  MyColor ret;
  ret.r = random(c.r);
  ret.g = random(c.g);
  ret.b = random(c.b);
  return ret;
}

void chaos(unsigned long t) {

  MyColor c0;
  t2rgb1(t, c0);
  MyColor c1;
  t2rgb1(t - 0.1 * alarm_fade_time, c1);
  MyColor c2;
  t2rgb1(t - 0.2 * alarm_fade_time, c2);
  MyColor c3;
  t2rgb1(t - 0.3 * alarm_fade_time, c3);

  setClockPixel(0, 0, randomish(c0));


  for (int i = 0; i < 12; i++) {
    setClockPixel(i, 1, randomish(c1));
    setClockPixel(i, 2, randomish(c2));
    setClockPixel(i, 3, randomish(c3));
  }

  //
  //  for (int i = 0; i < NLEDS; i++) {
  //    int r = random(c.r);
  //    int g = random(c.g);
  //    int b = random(c.b);
  //    strip.setPixelColor(i, doColor({r, g, b}));
  //  }

}

bool overload = false;
void fadeIn(unsigned long t) {


  if (readVcc() < LOW_VOLT_MILLIS) {
    if (!overload) DEBUGLN(" TOO MUCH LIGHT ARGGHHH ");
    overload = true;
    brightness *= .9;
  }

  if (!overload) {
    setBrightness(((float)t) / alarm_fade_time);
  }
}

void dispSunrise(unsigned long t) {

  unsigned long tX = t + (rate * rate * ALARM_FADE_IN_TIME) / 15.8;
  switch (sunriseType) {
    case 1:
      // DEBUGLN(tX);
      fadeIn(tX);
      hannah2(tX);
      break;
    case 2:
      fadeIn(tX);
      hannah(tX);
      break;

    case 3:
      fadeIn(t);
      rainbowCycleRad(t);
      break;

    //    case 4:
    //      fadeIn(t);
    //      vanilla(t);
    //      break;
    //
    case 4:
      fadeIn(tX);
      chaos(tX);
      break;

    case 5:
      fadeIn(t);
      rainbowCycle(t);

  }
  displayShow();
}


void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}


void displayVRTrain(int key) {

  setRadPixels(key, VR_COL1, 0x4);
  strip.show();
}


void displayPM(bool pm) {
  if (pm) strip.setPixelColor(0, doColor(TIME_COL));
  else strip.setPixelColor(0, doColor(BACK_COL));
}

void displayTimeHour(int hr) {
  setRadPixels(hr, TIME_COL, TIME_HOUR_PIX);
}


void displayAlarmHour(int hr) {
  setRadPixels(hr, ALARM_COL, ALARM_HOUR_PIX);
}


void displayTimeMinute(int mi) {
  setRadPixels(mi, TIME_COL, TIME_MINUTE_PIX);
}
void displayAlarmMinute(int mi) {

  setRadPixels(mi, ALARM_COL, ALARM_MINUTE_PIX);
}

void drawAlarm() {
  setRadPixels(alarm.Minute / 5, ALARM_COL, ALARM_MINUTE_PIX);
  setRadPixels(alarm.Hour, ALARM_COL, ALARM_HOUR_PIX);
}




void doAlarm() {
  unsigned long tNow = millis();

  alarm_fade_time = ALARM_FADE_IN_TIME * rate * rate;

  overload = false;

  setBrightness(0);

  while ( checkButtons() == 0 && !(visitOffVr()) ) {
    unsigned long t = millis() - tNow;
    
    if (t < alarm_fade_time)  {
      dispSunrise(t);
    } else if ( t > 60L*60*1000) {
      break;
    } else {
      unsigned long val = (t / 50);
      if ((val % 20) == 0 ) {
        dispSunrise(t);
      } else {
        displayClear();
        displayShow();
      }
    }
  }

  //DEBUGLN(" QUIT ALARM");
  doDisplay = true;
  displayClear();
  displayShow();
  waitForRelease();

}

void displayShow() {
  strip.show();
}


MyColor bit2col(int key) {
  MyColor c;
  if ( key & 1  ) c.r = 255;
  else c.r = 0;

  if ( key & 2  ) c.g = 255;
  else c.g = 0;

  if ( key & 4  ) c.b = 255;
  else c.b = 0;

  return c;
}

void displaySetScheme() {
  TIME_COL = bit2col(scheme1);
  ALARM_COL = bit2col(scheme2);
}

void displaySetSchemeVR(int scheme) {


  switch (scheme) {
    case 1:
      TIME_COL = {0, 0, 255};
      ALARM_COL = {0, 255, 0};
      return;

    case 2:
      TIME_COL = {0, 255, 0};
      ALARM_COL = {0, 0, 255};
      return;

    case 3:
      TIME_COL = {0, 255, 0};
      ALARM_COL = {255, 0, 0};
      return;

    case 4:
      TIME_COL = {255, 0, 255};
      ALARM_COL = {255, 255, 0};
      return;

    case 5:
      TIME_COL = {0, 0, 255};
      ALARM_COL = {255, 0, 255};
      return;

    case 6:
      TIME_COL = {255, 255, 255};
      ALARM_COL = {255, 0, 0};
      return;
  }
}


void displayAutoBright() {

  if (doDisplay) {
    float raw = analogRead(A6);
    float lraw = mapLight(raw);
    float bright = filter(lraw);
    setBrightness(bright / 255);
  } else {
    setBrightness(0);
  }

}


void display() {

  displayAutoBright();

  static int cnt = 0;

  displayClear();
  if ((cnt / 100) % 2) {
    drawTime();
    if (alarm.on) drawAlarm();
  } else {
    if (alarm.on) drawAlarm();
    drawTime();
  }
  cnt += 1 ;
  strip.show();
}

void setupDisplay() {
  strip.begin();
  setBrightness(0);
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(255);
}


void displayVRTrain(int key, TrainState state) {

  switch (state) {
    case WAIT:
      setRadPixels(key, VR_WAIT_COL, 0x4);
      break;
    case SPEAK:
      setRadPixels(key, VR_SPEAK_COL, 0x4);
      break;
    case NOMATCH:
      setRadPixels(key, VR_NOMATCH_COL, 0x4);
      break;
    case AGAIN:
      setRadPixels(key, VR_AGAIN_COL, 0x4);
      break;
    case NOVOICE:
      setRadPixels(key, VR_NOVOICE_COL, 0x4);
      break;
    case SUCCESS:
      setRadPixels(key, VR_SUCCESS_COL, 0x4);
      break;
  }
}

void displayVRTrained(uint8_t trainState[] ) {

  for (int i = 0; i < 12; i++) {
    if (trainState[i] == 0xF0) {
      continue;
    }

    switch (trainState[i]) {
      case 0x01:
        setRadPixels(i, VR_TRAINED_COL, 0x1);
        break;
      case 0x00:
        setRadPixels(i, VR_UNTRAINED_COL, 0x1);
        break;
      case 0xFF:
        setRadPixels(i, VR_ERROR_COL, 0x1);
        break;
      default:
        setRadPixels(i, VR_UNKNOWN_COL, 0x1);
        break;
    }
  }
}


