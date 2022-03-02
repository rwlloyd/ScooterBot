#include "Arduino.h"
#include "BTS7960.h"
#include "comms.h"
#include "config.h"
#include "movingAvg.h"
#include "PID_v1.h"

BTS7960 actuatorController(ACT_L_EN, ACT_R_EN, ACT_L_PWM, ACT_R_PWM);
int actuatorPosition;
movingAvg actuatorPositionAvg(4);                // define the moving average object

int actuatorMax = actuatorCentre - 90;
int actuatorMin = actuatorCentre + 90;
int vel = 200;

//Define Variables we'll be connecting to
double actuatorSetpoint, actuatorInput, actuatorOutputLeft, actuatorOutputRight;

//Specify the links and initial tuning parameters
double Kp = ACT_P, Ki = ACT_I, Kd = ACT_D;
PID leftPID(&actuatorInput, &actuatorOutputLeft, &actuatorSetpoint, Kp, Ki, Kd, DIRECT);
PID rightPID(&actuatorInput, &actuatorOutputRight, &actuatorSetpoint, Kp, Ki, Kd, REVERSE);
//int actuatorSetpoint;

void setupActuator(){
    pinMode(ACT_FB, INPUT);
    pinMode(ACT_R_EN, OUTPUT);
    pinMode(ACT_L_EN, OUTPUT);
    pinMode(ACT_R_PWM, OUTPUT);
    pinMode(ACT_L_PWM, OUTPUT);

    // Read the actuator position
    actuatorInput = analogRead(ACT_FB);
    // begine the position averaging.
    actuatorPositionAvg.begin();
    // set the inital setpoint to the centre position
    actuatorSetpoint = actuatorCentre;
    // Just to be on the safe side, make sur the actuator is disabled
    actuatorController.Stop();
    actuatorController.Disable();
    //turn the PID on
    leftPID.SetMode(AUTOMATIC);
    rightPID.SetMode(AUTOMATIC);
}

void stopActuator(){
    actuatorController.Stop();
    actuatorController.Disable();
}

void doActuatorControl(){
    leftPID.Compute();
    rightPID.Compute();
    if (!enable) {
        stopActuator();
    }
    else {
        actuatorController.Enable();
        actuatorSetpoint = map(actuatorSetpoint, 255, 0, actuatorMax, actuatorMin);
        actuatorPosition = analogRead(ACT_FB);
        actuatorInput = actuatorPositionAvg.reading(actuatorPosition);
        if (actuatorInput <= actuatorSetpoint + actuatorDeadband && actuatorInput >= actuatorSetpoint - actuatorDeadband) {
        //actuatorController.Stop();
        actuatorController.Disable();
        }
        else if (actuatorInput > actuatorSetpoint + actuatorDeadband) {
        actuatorController.Enable();
        actuatorController.TurnRight(actuatorOutputRight);
        }
        else if (actuatorInput < actuatorSetpoint - actuatorDeadband) {
        actuatorController.Enable();
        actuatorController.TurnLeft(actuatorOutputLeft);
    }
    }
}

