#!/usr/bin/env python3
import serial
import math
from time import sleep
import PG9038S as bt

print("    Differential Drive Remote Control for Serial-Curtis Bridge v1.3 and Generic Bluetooth Controller")
print("    Usage: Left Trigger = Toggle Enable; Left Joystick for left wheels motion and Right Joystick for right wheels motion")

# creates an object for the serial port
try:
    arduinoData = serial.Serial("/dev/ttyUSB1", 115200, timeout=1)
except:
    print("Arduino Failed to connect")
    pass

# So the direction in general can be reversed
direction = -1

message = []  # Seems to be necessary to have a placeholder for the message here
last_message = []

# creates an object for the bluetooth control
controller = bt.PG9038S("/dev/input/event1")

# Init values for enable and estop
estopState = False
enable = False
left_y = 128
right_y = 128

## Functions -----------------------------------------------------------------------

def rescale(val, in_min, in_max, out_min, out_max):
    """
    Function to mimic the map() function in processing and arduino.
    """
    return out_min + (val - in_min) * ((out_max - out_min) / (in_max - in_min))

def generateMessage(estopState, enable, right_vel, left_vel):
    """
    Accepts an input of two bools for estop and enable. 
    Then two velocities for right and left wheels between -100 and 100
    """
    # Empty list to fill with our message
    messageToSend = []
    # Check the directions of the motors, False = (key switch) forward, True = reverse
    # Reverse direction respect to joysticks e.g. left up = right reverse
    if right_vel < 0:
        left_direction = True
    else:
        left_direction = False
    if left_vel < 0:
        right_direction = True
    else:
        right_direction = False
    # Check to see if we're allowed to move. estop and enable
    if estopState or not enable:
        left_vel = 0
        right_vel = 0

    # Build the message. converting everything into positive integers
    messageToSend.append(int(estopState))
    messageToSend.append(int(enable))
    messageToSend.append(int(left_direction))
    messageToSend.append(abs(int(left_vel)))
    messageToSend.append(int(right_direction))
    messageToSend.append(abs(int(right_vel)))
    messageToSend.append(int(left_direction))
    messageToSend.append(abs(int(left_vel)))
    messageToSend.append(int(right_direction))
    messageToSend.append(abs(int(right_vel)))
    print("Sending: %s" % str(messageToSend))
    return messageToSend

def send(message_in):
    """
    Function to send a message_in made of ints, convert them to bytes and then send them over a serial port
    message length, 10 bytes.
    """
    messageLength = 10
    message = []
    for i in range(0, messageLength):
        message.append(message_in[i].to_bytes(1, 'little'))
    for i in range(0, messageLength):
        arduinoData.write(message[i])
    #print(message)

def receive():
    """
    Function to read whatever is presented to the serial port and print it to the console.
    Note: For future use: Currently not used in this code.
    """
    messageLength = len(message)
    last_message = []
    try:
        while arduinoData.in_waiting > 0:
            for i in range(0, messageLength):
                last_message.append(int.from_bytes(arduinoData.read(), "little"))
        #print("GOT: ", last_message)
        return last_message
    except:
        print("Failed to receive serial message")
        pass

# Main Loop
while True:
    try:
        newStates = controller.readInputs()
    except IOError:
        pass

    last_message = receive()
    #print(last_message)

    # enable = bool(newStates["trigger_l_1"])# Dead mans switch left trigger button

    #reset after estop
    if newStates["trigger_l_1"] == 1 and newStates["trigger_r_1"] == 1:
        estopState = False

    if newStates["button_left_xy"] == 1 or newStates["button_right_xy"] == 1:
        estopState = True
    
    if estopState == True:
        enable = False

    if newStates["trigger_r_2"] == 1:
        if estopState == False:
            enable = True
    else:
        enable = False

    if enable == True:  
        # Reverse direction respect to joysticks e.g. left up = right reverse
        right_y = newStates["left_y"] # Left joystick up / down
        left_y = newStates["right_y"] # Right joystick up / down
    
        # Calculate the final velocities rescaling the absolute value to between -1 and 1
        left_vel = rescale(left_y, 0,255,-1,1)
        right_vel = rescale(right_y, 255,0,-1,1)
    else:
        right_y = 128 # Left joystick up / down
        left_y = 128 # Right joystick up / down

        # Calculate the final velocities rescaling the absolute value to between -1 and 1
        left_vel = rescale(left_y, 0,255,-1,1)
        right_vel = rescale(right_y, 255,0,-1,1)

    # Build a new message with the correct sequence for the Arduino
    new_message = generateMessage(int(estopState), int(enable), right_vel, left_vel)
    #print(int(estopState), int(enable), right_vel, left_vel)
    # Send the new message
    send(new_message)
    # So that we don't keep spamming the Arduino....
    sleep(0.05)

