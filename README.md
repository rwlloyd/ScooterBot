# ScooterBot
A robot based on a three wheeled mobility scooter chassis. Drive motor via transaxle DC motor and Steering angle control
via 24V linear actuator with potentiometer feedback.

Rob Lloyd

Lincoln, UK. 2019 - 21

Power.

A 24VDC Supply from 2x 12V, 15Ah batteries supplies both BTS7968 Motor Drivers. 
A secondary 5V supply is taken from the primary 24V for Controller supply.

Electronics

A Raspberry Pi running Raspbian is used as the central controller. An arduino Nano microcontroller is 
used to provide PWM signals to 2xBTS7960 40A H-Bridge drivers. suitable transverse voltage suppression diodes should be used.
These drivers control a DC motor for forward and reverse motion and A linear actuator with potentiometer feedback.
Additionally, an electronic brake is energised with 24V via a 5V relay, controlled by the Arduino.

Modifications to Raspberry pi
--------------------------------
Username: pi
Password: Name1234

/home/pi/Scooterbot folder contains:

scooterbot.py - 	Main script that takes input from the bluetooth controller,
			        runs it through a basic kinematic model and outputs it to the arduino 
			        via serial.
                    Pairing devices with linux bluetoothctl. This has to be done manually during the setup.
                    https://raspberry-valley.azurewebsites.net/Map-Bluetooth-Controller-using-Python/
                    on windows...... http://pages.iu.edu/~rwisman/c490/html/pythonandbluetooth.htm ???
                    https://approxeng.github.io/approxeng.input/bluetooth.html

launcher.sh - 	Dont forget to make it executable! chmod a+x
		        Used to fire the main python script - may be redundent now we're using systemctrl 
		        to run a service. speaking of which

remoteControl.service - https://www.raspberrypi.org/documentation/linux/usage/systemd.md
			most importantly, you have to make sure that the service is launched fairly late in 
			the boot process. Otherwise the script cant find the device and shits itself. 
			Obviously this should be fixed in the script... but not right now? 

To add a service that is the last thing to start. Requires the Controller to be already paired
and automatically connect at startup (CONTROLLER ON AND SEARCHING BEFORE PI STARTUP).

sudo raspi-config
sudo apt-get update
sudo apt-get upgrade
sudo apt install python3-pip
pip3 install evdev
pip3 install pyserial

- Getting A bluetooth controller to automatically oconnect to a Pi

1. Controller on and searching for paired device
2. Power on pi

- setup:

sudo bluetoothctl
	scan on
	- get the mac address of the correct controller
	connect XX:XX:XX:XX:XX:XX
	pair XX:XX:XX:XX:XX:XX
	trust XX:XX:XX:XX:XX:XX
	exit

- To add a service and make it run on startup

sudo nano /etc/systemd/system/remoteControl.service

----------------------------------------------------------
[Unit]
Description=Service for Bluetooth Remote Control
After=getty.target

[Service]
ExecStart=sh launcher.sh
WorkingDirectory=/home/pi/scripts
StandardOutput=inherit
StandardError=inherit
Restart=always
User=pi

[Install]
WantedBy=multi-user.target

---------------------------------------------------------

sudo chmod a+r /etc/systemd/system/remoteControl.service

sudo systemctl daemon-reload

sudo systemctl start remoteControl.service
- check everything is working
sudo systemctl stop remoteControl.service
sudo systemctl enable remoteControl.service

sudo reboot
- everything should work

- to help debug:
sudo systemctl status remoteControl.service
- to tail the cmd line output of the service...
journalctl -f -u remoteControl.service 

Powering the pi
----------------
https://magpi.raspberrypi.org/articles/power-supply

GND - BRD6
5V - BRD2
