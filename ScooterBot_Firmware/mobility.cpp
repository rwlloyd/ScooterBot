#include "Arduino.h"
#include "BTS7960.h"
#include "comms.h"
#include "config.h"
#include "safety.h"

#include <movingAvg.h>                  // https://github.com/JChristensen/movingAvg

BTS7960 motorController(MOT_L_EN, MOT_R_EN, MOT_L_PWM, MOT_R_PWM);
int lastMotorSpeed;
int motorDesired;

// Somewhere to store variables
int motorSpeed;
movingAvg motorDesiredAvg(10);                // define the moving average object

void setupMobility(){
      //Brake Pin
  pinMode(BRAKE_PIN, OUTPUT);
  pinMode(MOT_R_EN, OUTPUT);
  pinMode(MOT_L_EN, OUTPUT);
  pinMode(MOT_L_PWM, OUTPUT);
  pinMode(MOT_R_PWM, OUTPUT);

  motorDesiredAvg.begin();
  motorController.Stop();
  motorController.Disable();
}

void stopMotor(){
    digitalWrite(BRAKE_PIN, enable);
    motorController.Stop();
    motorController.Disable();
}

void doMobilityControl(){
    if (!enable) {
        stopMotor();
    }
    else {
        digitalWrite(BRAKE_PIN, enable);
        motorController.Enable();
        // Motor
        motorDesired = motorDesiredAvg.reading(motorSpeed);
    
        //motorController.Enable();
        if (motorDesired == 128) {
            //motorController.Stop();
            //motorController.Disable();
            motorController.TurnRight(0);
        }
        if (motorDesired < 128) {
             motorController.Enable();
            motorController.TurnRight(map(motorSpeed, 128, 0, 0, speedLimit));
        }
        if (motorDesired > 128) {
            motorController.Enable();
            motorController.TurnLeft(map(motorSpeed, 128, 255, 0, speedLimit));
        }
    }
}

