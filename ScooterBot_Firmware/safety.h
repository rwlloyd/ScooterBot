#ifndef SAFETY_H
#define SAFETY_H
#include <Arduino.h>

extern bool error;
extern bool enable;

void startSafety();
void errorCheck();

#endif
