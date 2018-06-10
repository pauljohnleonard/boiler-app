#include <math.h>
#include "wifi.h"
#include "eeprom.h"


MyWifi wifi=MyWifi();

void setup() {

  delay(1000);

  Serial.begin(74880);
  Serial.println("Boiler App");
  Serial.print(F(__DATE__));
  Serial.print("  ");
  Serial.println(F(__TIME__));

  eeprom_setup();

  wifi.setupStatic();

  if (wifi.state < 0) {  //
    wifi.setupAP();
  }

}





void loop() {


  String  request = wifi.getCommand();

  if (request != "") {
    wifi.handleCommand(request);
    delay(1);
  }

}


