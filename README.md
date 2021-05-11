## ScooterBot

A robot based on a three wheeled mobility scooter chassis. Drive motor via transaxle DC motor and Steering angle control
via 24V linear actuator with potentiometer feedback.

Rob Lloyd

Lincoln, UK. 2019 - 21

## Power.
* A 24VDC Supply from 2x 12V, 15Ah batteries supplies both BTS7968 Motor Drivers. 
* A secondary 5V supply is taken from the primary 24V for Controller supply.
* Read About powering the pi via the GPIO pins - https://magpi.raspberrypi.org/articles/power-supply

GND | BRD6 
5V | BRD2 / BRD4
## Electronics
A Raspberry Pi running Raspbian is used as the central controller. An arduino Nano microcontroller is 
used to provide PWM signals to 2xBTS7960 40A H-Bridge drivers. suitable transverse voltage suppression diodes should be used.
These drivers control a DC motor for forward and reverse motion and A linear actuator with potentiometer feedback.
Additionally, an electronic brake is energised with 24V via a 5V relay, controlled by the Arduino.

## Modifications to Raspberry pi
* Username: pi
* Password: Name1234

```/home/pi/Scooterbot``` folder contains:

scooterbot.py | Main script that takes input from the bluetooth controller,runs it through a basic kinematic model and outputs it to the arduino via serial.

launcher.sh | Dont forget to make it executable! chmod a+x
		        Used to fire the main python script - may be redundent now we're using systemctrl 
		        to run a service. speaking of which

remoteControl.service | https://www.raspberrypi.org/documentation/linux/usage/systemd.md
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

## Getting A bluetooth controller to automatically to connect to a Pi 
[This](https://raspberry-valley.azurewebsites.net/Map-Bluetooth-Controller-using-Python/), [this](http://pages.iu.edu/~rwisman/c490/html/pythonandbluetooth.htm) and [this](https://approxeng.github.io/approxeng.input/bluetooth.html) helped. but basically;

1. Controller on and searching for paired device
2. Power on pi

## More detailed...:

	sudo bluetoothctl

Get the bluetooth to have a look around

	scan on 

Write down the mac address of the correct controller. (Tab-complete works though)

	connect XX:XX:XX:XX:XX:XX
	pair XX:XX:XX:XX:XX:XX
	trust XX:XX:XX:XX:XX:XX
	exit

You should now have a new device in your inputs directory. Check with;

	ls /dev/input/


To add the service and make it run on startup

	sudo touch /etc/systemd/system/remoteControl.service
	sudo nano /etc/systemd/system/remoteControl.service

Add the following lines to the file

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

Make the file executable

	sudo chmod a+r /etc/systemd/system/remoteControl.service

Reload systemctl

	sudo systemctl daemon-reload

Start the new service

	sudo systemctl start remoteControl.service

Check everything is working

	sudo systemctl stop remoteControl.service
	sudo systemctl enable remoteControl.service
	sudo reboot

Everything should work without errors

To help debugging:

	sudo systemctl status remoteControl.service

To tail the cmd line output of the service...

	journalctl -f -u remoteControl.service 




# ROS-ification 10/5/21

Where to start, Let's install ROS on the Raspberry Pi.

NO! First check that the git is all upto date and such.... It is btw.

Now, proceed with no fear that it's been, oh, only a month or two since I worked on this. Seems longer.... Anyway, first, let's try to be lazy and install everything using the LCAS script from

https://github.com/LCAS/rosdistro/wiki#batch-install

Oh buggeration. That means it needs to be running on ubuntu, not Raspbian. will that work? Are they near enough the same? Fuck it.

	To make it simple, just copy this and paste in your shell:

		sudo ls (this is just to cache you admin password for the next steps)
		sudo apt-get update && sudo apt-get install curl (curl is required for the next step)
		curl https://raw.githubusercontent.com/LCAS/rosdistro/master/lcas-rosdistro-setup.sh | bash - 
		
	(should install everything required)

	If the above has worked, there's no need to read any further.

Well, that's a negatory... and really, we want this on a pi4 anyway and that linux image is waaaaay past 18.04 so maybe it's time to look to the wider internet.

*Googles "raspberry pi 4 ros melodic image"*

Ooh.... looky here Susan.

http://wiki.ros.org/ROSberryPi/Installing%20ROS%20Melodic%20on%20the%20Raspberry%20Pi

or better still, heres the raspberry pi 3 ubuntu image:

https://cdimage.ubuntu.com/releases/18.04.5/release/ubuntu-18.04.5-preinstalled-server-armhf+raspi3.img.xz

from this page

https://cdimage.ubuntu.com/releases/18.04.5/release/

Then we'll try running the LCAS script again....