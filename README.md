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
Open the serial monitor. Set the line break to **new line** and the speed to **9600 baud**

If you did not remove the sample code the printout will look like
````
Motor A
Motor B
Motor C
clearMove()
PowerOff
````
after uploading the sketch.

### commands
The serial interface allows the following command to be send to the multistepper:

**MD** Letter pin1 pin2 pin3 pin4 [homeSwitchPin]  
*Motor Definition: Adds a new motor to the multistepper*  

**ML**  
*Motor List: Prints a list of the known motors*  

**GA** Letter Position [Letter2 Position2 ...] [FD StepsPerSecond]  
*Go Absolute: Defines a motion and starts it. The motor movements are synchronized to start and finish synchronously.*  

**MOFF**  
*Motors Off: Switches off all motor coils. The 28BYJ-48 tends to heat up very quickly so switching the magnetic coils off is recommended. For small (lego) models the gear ratio (~1:64) is large enough for the self-locking of the gear to hold the position.*

**APO** 0/1  
*Auto Power Off after Motion: Shuts down the coil current after completion of each **GA** command*

### examples

*Motor List*  
Enter **ML** and press <enter>
````
ML
Parsing command ML
Info Motor 0 [A]:
 Coil Pins: 4 5 6 7
 No End stop (home) pin.
 Current position: 0
Info Motor 1 [B]:
 Coil Pins: 8 9 10 11
 No End stop (home) pin.
 Current position: 0
Info Motor 2 [C]:
 Coil Pins: 22 23 24 25
 No End stop (home) pin.
 Current position: 0
OK
````

*Adding a new (4th) motor:*
I connected a fourth motor to the pins 34..37, and want it to be known as motor "D":  
**MD D 34 35 36 37** <enter>

````
MD D 34 35 36 37
Parsing command MD D 34 35 36 37
Info: No home pin defined pin.
OK
````
now *ML* prints:
````
ML
Parsing command ML
Info Motor 0 [A]:
 Coil Pins: 4 5 6 7
 No End stop (home) pin.
 Current position: 0
Info Motor 1 [B]:
 Coil Pins: 8 9 10 11
 No End stop (home) pin.
 Current position: 0
Info Motor 2 [C]:
 Coil Pins: 22 23 24 25
 No End stop (home) pin.
 Current position: 0
Info Motor 3 [D]:
 Coil Pins: 34 35 36 37
 No End stop (home) pin.
 Current position: 0
OK
````

*Coordinated motion of two motors:*
In this example, A and C motors are to be moved, and they started both at position 0.
````
GA A 1000 C 2000
Parsing command GA A 1000 C 2000
OK: motor A is motor #0
OK: position 1000
Motor 0
OK: motor C is motor #2
OK: position 2000
Motor 2
0: Steps 1000
1: Steps 0
2: Steps 2000
3: Steps 0
0 done: 1000
2 done: 2000
OK
````



## Work in progress
 - home switches (end stops)
 - precision with large numbers of steps. The floating point arithmetic currently used is not precise enough. Sometimes a motor has to make some steps after the others have finished to reach its final position.




# DISCLAIMER
This is a) work in progress b) comes with absolutely no warranty in any way, to the extend legally possible.
