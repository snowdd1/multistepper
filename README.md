# Multistepper
This short ARDUINO sketch allows controlling multiple stepper motors at once. It is set up for 28BYJ-48 stepper motors.

I needed a program that allows the control of more than one motor simultaneously to use the cheap steppers I got (28BYJ-48) with the 
[LEGO-compatible housing](http://www.thingiverse.com/thing:1674077) for a LEGO technic robot arm that I built with my son.

The requirements were:
- multiple motors move simultaneously with one command (comparable to 3D-printer *G1* command)
- feeding of new commands to the arduino via serial/USB (to later add a raspberry with user interface)
- as flexible as possible with as little code as possible

## Hardware
- Stepper motors [28BYJ-48](http://www.instructables.com/id/BYJ48-Stepper-Motor/) with ULN2003 driver boards
- Arduino UNO or MEGA (these I tested)
- jumper wires  

*optional*:
- *LEGO housings for the motors*
- *zillions of LEGO technic sets* :smile:

## Install
- just load the myStepper.ino into your arduino board
- connect the driver boards to the arduino and to a power source
  If you did not change the sample code from [line 632](https://github.com/snowdd1/multistepper/blob/master/myStepper.ino#L632) you need to
  connect the first 3 motors as follows:
  - Mot1: Pins 4,5,6,7  
  - Mot2: Pins 8,9,10,11  
  - Mot3: Pins 22,23,24,25 (Arduino MEGA only)  
  You are free to change the pins as needed!

## Usage
Open the serial monitor. If you did not remove the sample code it will look like
````
Motor A
Motor B
Motor C
clearMove()
PowerOff
````


## Work in progress
 - home switches (end stops)




# DISCLAIMER
This is a) work in progress b) comes with absolutely no warranty in any way, to the extend legally possible.
