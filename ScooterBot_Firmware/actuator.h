#ifndef ACTUATOR_H
#define ACTUATOR_H
#include <Arduino.h>

extern double actuatorSetpoint;

void setupActuator();
void doActuatorControl();
void stopActuator();

#endif