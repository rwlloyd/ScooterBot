


//ESTOP INPUT NOT DONE YET


#include "BTS7960.h"
#include <PID_v1.h>
#include <movingAvg.h>                  // https://github.com/JChristensen/movingAvg

// for Brake
const uint8_t BRAKE_PIN = 11;

// for motor, left is backwards
const uint8_t MOT_R_EN = 3;
const uint8_t MOT_L_EN = 4;
const uint8_t MOT_L_PWM = 6;
const uint8_t MOT_R_PWM = 5;
#define MOT_L_IS A1
#define MOT_R_IS A2

BTS7960 motorController(MOT_L_EN, MOT_R_EN, MOT_L_PWM, MOT_R_PWM);

int motorCentre = 128;
int motorMin = 50;
int motorMax = 255;
int speedLimit = 128;
int lastMotorSpeed;
int motorDesired;

// For actuator, extend is....
const uint8_t ACT_R_EN = 7;
const uint8_t ACT_L_EN = 8;
const uint8_t ACT_R_PWM = 9;
const uint8_t ACT_L_PWM = 10;
const uint8_t ACT_FB = A0;

BTS7960 actuatorController(ACT_L_EN, ACT_R_EN, ACT_L_PWM, ACT_R_PWM);
int actuatorPosition;
movingAvg actuatorPositionAvg(4);                // define the moving average object
int actuatorCentre = 263;
int actuatorDeadband = 1; //Stops the actuator hunting quite so much
int actuatorMax = actuatorCentre - 90;
int actuatorMin = actuatorCentre + 90;
int vel = 200;

//Define Variables we'll be connecting to
double actuatorSetpoint, actuatorInput, actuatorOutputLeft, actuatorOutputRight;

//Specify the links and initial tuning parameters
double Kp = 3, Ki = 0.1, Kd = 0.1;
PID leftPID(&actuatorInput, &actuatorOutputLeft, &actuatorSetpoint, Kp, Ki, Kd, DIRECT);
PID rightPID(&actuatorInput, &actuatorOutputRight, &actuatorSetpoint, Kp, Ki, Kd, REVERSE);
//int actuatorSetpoint;

// Comms
// length of data packet. 2*motor speed + steering position
const int messageLength = 4;
// Array for the received message
int received[messageLength];
// Flag to signal when a message has been received
bool commandReceived = false;

// VAriables that deal with checking the time since the last serial message
// If we lose connection, we should stop
unsigned long lastMillis;
unsigned long currentMillis;
const unsigned long period = 250;  //the value is a number of milliseconds, ie 2s

// we need to know if there is an error state from anywhere....
boolean error = false;
bool enable = false;
// Somewhere to store variables
int motorSpeed;
movingAvg motorDesiredAvg(10);                // define the moving average object

void setup() {
  //Brake Pin
  pinMode(BRAKE_PIN, OUTPUT);
  pinMode(ACT_FB, INPUT);
  pinMode(ACT_R_EN, OUTPUT);
  pinMode(ACT_L_EN, OUTPUT);
  pinMode(ACT_R_PWM, OUTPUT);
  pinMode(ACT_L_PWM, OUTPUT);

  pinMode(MOT_R_EN, OUTPUT);
  pinMode(MOT_L_EN, OUTPUT);
  pinMode(MOT_L_PWM, OUTPUT);
  pinMode(MOT_R_PWM, OUTPUT);

  actuatorInput = analogRead(ACT_FB);
  actuatorPositionAvg.begin();
  actuatorSetpoint = actuatorCentre;
  motorDesiredAvg.begin();
  motorController.Stop();
  motorController.Disable();
  actuatorController.Stop();
  actuatorController.Disable();

  // Setup serial connection, announce device and initiate dacs
  Serial.begin(115200);
  delay(100);
  //Serial.write(49);                 // Announce the controller to the PC with '1'
  //Serial.write(10);
  //Serial.write(13);
  //turn the PID on
  leftPID.SetMode(AUTOMATIC);
  rightPID.SetMode(AUTOMATIC);
  //Serial.write(50);                 // Announce the setup complete with '2'
  lastMillis = millis();            // Record the time for connection checking
}

void loop() {
  checkConnection();
  leftPID.Compute();
  rightPID.Compute();
  if (commandReceived) {
    processSerialCommand();
  }
  if (!enable) {
    digitalWrite(BRAKE_PIN, enable);
    motorController.Stop();
    motorController.Disable();
    actuatorController.Stop();
    actuatorController.Disable();
  }
  else {
    digitalWrite(BRAKE_PIN, enable);
    motorController.Enable();
    actuatorController.Enable();
    //Actuator
    actuatorSetpoint = map(received[3], 255, 0, actuatorMax, actuatorMin);
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

    // Motor

    motorDesired = motorDesiredAvg.reading(received[2]);
    
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
  //delay(10);
}





// ======================== FUNCTIONS =====================================
// function to check the time since the last serial command
void checkConnection() {
  currentMillis = millis();
  if (currentMillis - lastMillis >= period) {
    enable = false;
    error = true;
  }
}

// When new characters are received, the serialEvent interrupt triggers this function
void serialEvent()   {
  // Read the Serial Buffer
  for (int i = 0; i < messageLength; i++) {
    received[i] = Serial.read();
    delay(1);
  }
  // Change the flag because a command has been received
  commandReceived = true;

  // Record the time
  lastMillis = millis();
}

// Function to split up the received serial command and set the appropriate variables
void processSerialCommand() {
  error = bool(received[0]);                                  // Error Flag
  enable = bool(received[1]);                          // Motor Enable Flag
  // Motor Directions
  motorSpeed = int(received[2]);
  actuatorSetpoint = int(received[3]);

  for (int i = 0; i < messageLength; i++) {
    Serial.write(received[i]);
  }
  //Serial.write(13);
  commandReceived = false;
}