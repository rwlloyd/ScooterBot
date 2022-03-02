#include "arduino.h"
#include "config.h"
#include "safety.h"

// we need to know if there is an error state from anywhere....
bool enable = false;
bool error = false;
//bool connectionEstablished = false;

void startSafety(){
  pinMode(errorPin, INPUT_PULLUP);
}

void errorCheck(){
  if (digitalRead(errorPin) == HIGH){
    error == true;
    //motorsEnabled = false;
  }
  else{
    error == false;
  }
}
