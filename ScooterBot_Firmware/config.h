#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// for Brake
#define BRAKE_PIN 11

#define errorPin 2

// for motor, left is backwards
#define MOT_R_EN 3
#define MOT_L_EN 4
#define MOT_L_PWM 6
#define MOT_R_PWM 5
#define MOT_L_IS A1
#define MOT_R_IS A2

#define motorCentre 128
#define motorMin 50
#define motorMax 255
#define speedLimit 128

// For actuator, extend is....
#define ACT_R_EN 7
#define ACT_L_EN 8
#define ACT_R_PWM 9
#define ACT_L_PWM 10
#define ACT_FB A0

#define ACT_P 3
#define ACT_I 0.1
#define ACT_D 0.1

#define actuatorCentre 263
#define actuatorDeadband 1              //Stops the actuator hunting quite so much

// Communication 
#define MESSAGELENGTH 4
#define BAUDRATE 115200
#define COMCHECKTIME 250

#endif