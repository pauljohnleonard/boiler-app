
#include "VoiceRecognitionV3.h"
#include "display.h"
#include "ui.h"
#include "vr.h"
#include "global.h"
#include "debug.h"




const char  speakNow[] PROGMEM = "Speak now";
const char noVoice[] PROGMEM = "No voice";
const char cantMatch[] PROGMEM = "Cann't matched";
const char speakAgain[] PROGMEM = "Speak again";
const char success[] PROGMEM = "Success";

enum CmdState {NONE,TOP,NUMBER};

// Commands 1-6    numbers
// 7 ALARM   toggle
// 8 OFF2 toggle   
// 9 OFF1
// 10 RATE      1-6
// 11 SUNRISE   1-6
//  0 SCHEME    1-6

#define QUIT_WAIT_LEN 6000

uint8_t topMenu[6]={6,7,8,9,10,11};
uint8_t digitsMenu[6]={1,2,3,4,5};

// VR

#define CMD_BUF_LEN      64+1
#define CMD_NUM          10


CmdState cmdState=NONE;

VR myVR(11, 12);   // 11:RX 12:TX, you can choose your favourite pins.

//---- VR CODE
uint8_t buf[255];
uint8_t records[7]; // save record


//
///**
//    @brief clear recognizer.
//    @retval  0 --> success
//            -1 --> failed
//*/
//void send_pkt(uint8_t cmd, uint8_t *buf, uint8_t len)
//{
//
//  
//  while(1) {  
//    int n=myVR.available();
//    if(n == 0) break;
//    DEBUG(n);
//    myVR.read();
//  }
//  
//  DEBUG("FF");
//  myVR.write(FRAME_HEAD);
//  myVR.write(len+2);
//  myVR.write(cmd);
//  myVR.write(buf, len);
//  myVR.write(FRAME_END);
//}
//
//
//int clear()
//{  
//  int len;
//  uint8_t vr_buf[32];
//
//  send_pkt(FRAME_CMD_CLEAR, 0, 0);
//  DEBUG("ZZZ");
//  len = myVR.receive_pkt(vr_buf);
//  if(len<=0){
//    return -1;
//  }
//
//  if(vr_buf[2] != FRAME_CMD_CLEAR){
//    return -1;
//  }
//  //DBGLN("VR Module Cleared");
//  return 0;
//}
//

void loadTop(bool force) {
  if (!force and (cmdState == TOP)) return; 
  DEBUGLN(" LOAD TOP");
  int ret=myVR.clear();
  if (ret != 0) DEBUGLN(" CLEAR FAILED ");
  ret=myVR.load(topMenu,6,buf);
  if (ret < 0) DEBUGLN(" LOAD FAILED ");
  cmdState=TOP;
}


void loadDigits(bool force) {
  if (!force and (cmdState == NUMBER)) return;
  myVR.clear();
  myVR.load(digitsMenu,6,buf);
  cmdState=NUMBER;
}

void loadNone(bool force) {
  myVR.clear();
  cmdState=NONE;  
}

void doCmd(uint8_t key1,uint8_t key2) {

  switch(key1) {
    case ALARM:
      alarm.on = !alarm.on;
      saveToEEPROM();
      return;
    case SUNRISE:
      sunriseType=key2;
      loadTop(false);
      saveToEEPROM();
      doAlarm();
      return;
    case OFF1:
    case OFF2:
      doDisplay = ! doDisplay;
      return;
    case SCHEME:

      displaySetSchemeVR(key2);
      saveToEEPROM();
      return;
    case RATE:
      rate=min(6,key2);
      loadTop(false);
      saveToEEPROM();
      doAlarm();
      return;
  }
  
}

bool visitOffVr() {
  
  int ret = myVR.recognize(buf, 20);
  if (ret <= 0 ) return false;
  int key1=buf[1];
  //printCmd(key1);
  if (key1 == OFF1 || key1 == OFF2) return true;
  return false;
  
}

void visitVR(void)
{
// Commands 1-6    numbers
// 7 ALARM   toggle
// 8 RATE 1-6   
// 9 OFF1
// 10 OFF1
// 11 SUNRISE   1-6 
//  0 SCHEME    1-6
  /** try to receive recognize result */
  int ret;

  loadTop(false);
  
  if (!voiceReco) return;
  
  ret = myVR.recognize(buf, 0);
  
  if (ret <= 0 ) return;
 
  int key1=buf[1];
  
  displayClear();
  
  switch(key1) {
    case ALARM:
    case OFF1:
    case OFF2:
      displayClear();
      displayVRTrain(key1,SPEAK);
      displayShow();
      doCmd(key1,-1);
      delay(300);
      return;
 
    case SUNRISE:
    case SCHEME:
    case RATE:
    
      displayClear();
      displayVRTrain(key1,AGAIN);
      displayShow();
      loadDigits(false);
      ret = myVR.recognize(buf, 1500);
      if ( ret > 0 ) {
          int key2=buf[1];
          displayVRTrain(key2,SPEAK);
          displayShow();
          delay(300);
          doCmd(key1,key2);
      } else {
        if (key1 == SUNRISE) {
          loadTop(false);
          doAlarm();  
        }
      }
      loadTop(false);
  }
          
        
  loadTop(false);
  return;
}


void setupVR() {
  
  myVR.begin(9600);
  delay(50);
  loadTop(true);
  voiceReco=false;
  
}


void resetVR() {
  for (int i=0;i<12;i++) trainState[i]=0;
  saveToEEPROM();
}

int trainRecord(int key)
{
  int ret;
  uint8_t vr_buf[32];
  unsigned long start_millis;
  uint8_t records[1];

  myVR.send_pkt(FRAME_CMD_TRAIN, records, 1);

  start_millis = millis();

  while (1) {

    if (checkButtons()) {
      
      return -1;
    }
    
    char buff[20];
    ret = myVR.receive_pkt(vr_buf,1000);
    const char *label=(const char *)(vr_buf + 4);
    int n=ret-6;

 
    if (ret > 0) {
      //Serial.println(label);
      //Serial.println(n);
      switch (vr_buf[2]) {
        case FRAME_CMD_PROMPT:
          if (strncmp_P(label,speakNow,n)==0 ){
            displayVRTrain(key, SPEAK);
              
          } else if (strncmp_P(label,noVoice,n)==0){
            displayVRTrain(key,NOVOICE); 
            displayShow();
            delay(400); 
            return -1;     // ABORT

                  
          }else if (0==strncmp_P(label,cantMatch,n)){
            displayVRTrain(key,NOMATCH);
            displayShow();
            delay(300); 
            
          }else if (0==strncmp_P(label, speakAgain,n)){
            displayVRTrain(key,AGAIN);
            delay(300);
             
          }else if (0==strncmp_P(label, success,n)) {
       //     DEBUG(" SUCCESS ");
            trainState[key]=0x01;  
            displayVRTrained(trainState);
            displayVRTrain(key,SUCCESS);
            displayShow();
            saveToEEPROM();
            delay(400);
            ret = myVR.receive_pkt(vr_buf);        
            return 0; 
          }
          displayShow();
          break;
        case FRAME_CMD_TRAIN:
          if (buf != 0) {
            memcpy(buf, vr_buf + 3, vr_buf[1] - 2);
            return vr_buf[1] - 2;
          }
          trainState[key]=0x01;          
          return 0;
        default:
          break;
      }
      start_millis = millis();
    }
    if (millis() - start_millis > 8000) {
      return -2;
    }
  }
  return 0;
}


void trainVR() {

  int state = 0;
  
  displayVRTrained(trainState);
  displayShow();
  waitForRelease();
 
  unsigned long tStart = millis();
  while ((millis()-tStart) < QUIT_WAIT_LEN) {
    
    int  n = checkButtons();
    
    if ( n > 0) {
      for (int i = 0 ; i < 12; i++) {
        if (buts[i]) {
          displayVRTrain(i, WAIT);
          displayShow();
          waitForRelease();
          trainRecord(i);
          cmdState=NONE;
        }
      }
      tStart = millis();
    
    } 
    displayClear();
    displayVRTrained(trainState);
    displayShow();
    waitForRelease();
    delay(30);
  
  }
  
  loadTop(true);

}



