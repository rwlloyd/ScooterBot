#ifndef COMMS_H
#define COMMS_H
#include "Arduino.h"
#include "safety.h"

void startComms();
void checkComms();
void serialEvent();
void processSerialCommand();

#endif
