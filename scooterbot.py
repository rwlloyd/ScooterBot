#!/usr/bin/env python3
import serial
import math
from time import sleep
import PG9038S as bt

# creates an object for the serial port
try:
    arduinoData = serial.Serial("/dev/ttyUSB0", 115200, timeout=1)
    #arduinoData = serial.Serial("/dev/ttyS0", 115200, timeout=1)
except:
    print("Arduino Failed to connect")
    pass

messageLength = 4
message = []
message_to_send = []
last_message = []

# creates an object for the bluetooth control
controller = bt.PG9038S("/dev/input/event1")

# Init values for enable and estop
estopState = False
enable = False
x = 128
y = 128

def send(message_to_send):
    """
    Function to send a message_to_send made of ints, convert them to bytes and then send them over a serial port
    message length, 10 bytes.
    """
    messageLength = 4
    message = []
    try:
        for i in range(0, messageLength):
            message.append(message_to_send[i].to_bytes(1, 'little'))
        for i in range(0, messageLength):
            arduinoData.write(message[i])
        #print("SENT:", message)
    except:
        print("Failed to send serial message")
        pass

def receive():
    """
    Function to read whatever is presented to the serial port and print it to the console.
    Note: For future use: Currently not used in this code.
    """
    messageLength = 4
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

def generateMessage(estopState, enable, speed, steer):
    messageToSend = []
    # Build the message. converting everything into positive integers
    messageToSend.append(int(estopState))
    messageToSend.append(int(enable))
    messageToSend.append(int(speed))
    messageToSend.append(int(steer))
    print(messageToSend)
    return messageToSend

while True:
    try:
        newStates = controller.readInputs()
    except IOError:
        pass

    #print(controller.printStates())

    last_message = receive()
    print(last_message)

    #reset after estop
    if newStates["trigger_l_1"] == 1 and newStates["trigger_r_1"] == 1:
        estopState = False
    # trigger estop
    if newStates["button_left_xy"] == 1 or newStates["button_right_xy"] == 1:
        estopState = True
    # failsafe for the enable state while estopped
    if estopState == True:
        enable = False
    # handle the enable button input
    if newStates["trigger_r_2"] == 1:
        if estopState == False:
            enable = True
    else:
        enable = False
    # handle the throttle ans steering inputs
    if enable == True:
        #x = newStates["right_x"]
        #y = newStates["left_y"]
        x =   newStates["left_x"]
        y =  newStates["left_y"]
    else:
        x = 128
        y = 128
    # generate the nessage
    new_message = generateMessage(estopState, enable, y, x)
    # send the message
    send(new_message)

    #message = None
    # last_message = None
    sleep(0.1)
