#include "config.h"
#include "safety.h"
#include "actuator.h"
#include "mobility.h"
// So we can use serialEvent().
#include "Arduino.h" 
// Setup serial communication bytes
// length of data packet. Just the number of bytes for now
const int messageLength = MESSAGELENGTH;
const int baudRate = BAUDRATE;
// Array for the received message
int received[MESSAGELENGTH];
// Flag to signal when a message has been received
bool commandReceived = false;

// VAriables that deal with checking the time since the last serial message
// If we lose connection, we should stop
unsigned long lastMillis;
unsigned long currentMillis;
const int period = COMCHECKTIME;  //the value is a number of milliseconds, ie 2s

void startComms() {
  // Start serial comms
  Serial.begin(BAUDRATE);
  // Give it a chance to settle. using delay() might give you grief if porting to a different ucontroller
  delay(50);
  Serial.println(0);
}

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
  // Do something with the received message here

  error = bool(received[0]);                                  // Error Flag
  enable = bool(received[1]);                          // Motor Enable Flag
  // Motor Directions
  motorSpeed = int(received[2]);
  actuatorSetpoint = int(received[3]);
  
  
  // Chirp the message back just because.
  for (int i = 0; i < messageLength; i++) {
    Serial.write(received[i]);
  }
  // Allow a new message
  commandReceived = false;
    // Record the time for connection checking
  lastMillis = millis();    
}

void checkComms() {
  if (commandReceived == true)   {                  // This code is executed in non interupt time only when a new command has been recieved
    // A new command has been recieved when a \n or \r character is recieved.
    processSerialCommand();                        // Process the command
  }
}




