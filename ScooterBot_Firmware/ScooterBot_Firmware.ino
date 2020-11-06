
#include "BTS7960.h"

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

// For Motor, extend is....
const uint8_t ACT_R_EN = 7;
const uint8_t ACT_L_EN = 8;
const uint8_t ACT_R_PWM = 9;
const uint8_t ACT_L_PWM = 10;

BTS7960 actuatorController(L_EN, R_EN, L_PWM, R_PWM);
int actuatorCentre = 300;
int actuatorMin = 300;
int actuatorMax = 200;
int vel = 100;

//Define Variables we'll be connecting to
double actuatorSetpoint, actuatorInput, actuatorOutputLeft, actuatorOutputRight;

//Specify the links and initial tuning parameters
double Kp = 1.5, Ki = 0.1, Kd = 0.1;
PID leftPID(&actuatorInput, &actuatorOutputLeft, &actuatorSetpoint, Kp, Ki, Kd, DIRECT);
PID rightPID(&actuatorInput, &actuatorOutputRight, &actuatorSetpoint, Kp, Ki, Kd, REVERSE);

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
bool error = false;

void setup(){
  //Brake Pin
  pinMode(BRAKE_PIN, OUTPUT);
  pinMode(FB, INPUT);
  pinMode(POT, INPUT);

  actuatorInput = analogRead(ACT_FB);
  actuatorSetpoint = analogRead(POT);

  //turn the PID on
  leftPID.SetMode(AUTOMATIC);
  rightPID.SetMode(AUTOMATIC);

    // Setup serial connection, announce device and initiate dacs
    Serial.begin(115200);
    delay(100);
    Serial.write(49);                 // Announce the controller to the PC with '1'
    Serial.write(10);
    Serial.write(13);

    Serial.write(50);                 // Announce the setup complete with '2'
    lastMillis = millis();            // Record the time for connection checking
}

void loop(){
    
    checkConnection();

    if (enable){
        digitalWrite(BRAKE_PIN, enable)
        motorController.Enable()
        actuatorController.Enable()
    }
    else {
      digitalWrite(BRAKE_PIN, enable)
      motorController.Stop()
      motorController.Disable()
      actuatorController.Stop()
      actuatorController.Disable()
    }


    
}

// ======================== FUNCTIONS =====================================
// function to check the time since the last serial command
void checkConnection() {
  currentMillis = millis();
  if (currentMillis - lastMillis >= period) {
    bool enable = false;
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
  bool error = bool(received[0]);                                  // Error Flag
  bool enable = bool(received[1]);                          // Motor Enable Flag
  // Motor Directions
  bool motorSpeed = int(received[2]);
  bool actuatorSetpoint = int(received[3]);

  for (int i = 0; i < messageLength; i++) {
    Serial.write(received[i]);
  }
  Serial.write(13);
  commandReceived = false;

  setOutputs();
}

void setOutputs(){
    actuatorPosition = analogRead(ACT_FB)
    actuatorSetpoint = int(received[3])
    leftPID.Compute();
    rightPID.Compute();
    if (actuatorSetpoint > actuatorPosition) {
      motorController.TurnLeft(OutputLeft);
    }
    else if (actuatorSetpoint < actuatorPosition) {
      motorController.TurnRight(OutputRight);
    }
}
