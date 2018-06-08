#ifndef _VR_H
#define _VR_H



#define COMMAND  0

#define ALARM 6
#define OFF1  7
#define OFF2  8
#define RATE  9
#define SUNRISE 10
#define SCHEME  11

void visitVR();
void setupVR();
void trainVR();
bool visitOffVr();
void resetVR();
void loadTop(bool force);

enum  TrainState {WAIT,SPEAK,NOVOICE,AGAIN,NOMATCH,SUCCESS};
 

#endif


