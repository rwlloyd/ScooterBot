#include "comms.h"
#include "config.h"
#include "mobility.h"
#include "actuator.h"
#include "safety.h"

void setup() {
  // Setup serial connection, announce device and initiate dacs
  startComms();
  setupActuator();
  setupMobility(); 
}

void loop() {
  //checkConnection();
  checkComms();
  if (!enable) {
    stopMotor();
    stopActuator();
  }
  else {
    doActuatorControl();
    doMobilityControl();
  }
}



