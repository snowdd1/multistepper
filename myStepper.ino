
#define maxMotors 4
#define minWait 1.6 // milliseconds between steps (=1600 microseconds)
#define ulongMax  4294967295 // for micros() overflow

// magentic coil powering for 28byi-48 stepper motor with ULN2003 chip
// 1 is HIGH is power on
const int coils[4][4] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

struct motdef {
  int m_pin1 = 0;
  int m_pin2 = 0;
  int m_pin3 = 0;
  int m_pin4 = 0;
  int m_zero_pin = -1;
  unsigned long m_position = 0;
};

/*
   Stepmotor controls ONE motor

   Constructors
   Stepmotor(void) ==> initMotor(int pin1, int pin2, int pin3, int pin4, int home_switch)
   StepMotor(int pin1, int pin2, int pin3, int pin4, int home_switch)

   boolean step(boolean forward) - moves one step
   boolean power_off(void) - disables the stepper magnetic coils (energy & warmth!)
   boolean home(void)--> for use with home switch connector

   long getPosition() - returns current position of motor

*/
class Stepmotor
{
    // coil connections
    int m_pin1 = 0;
    int m_pin2 = 0;
    int m_pin3 = 0;
    int m_pin4 = 0;
    boolean init = false;

    // optional home position switch
    int m_zero_pin = -1;

    // current position in steps
    unsigned long m_position = 0;

    // minimum pause between two steps (depends on voltage for example!)
    // usually 1.6 millisecs are sufficient
    float m_min_time_milli = 1.6;


    // constructor
    //   home_switch can be 0 or less if no zero position switch is connected
  public:
    Stepmotor(int pin1, int pin2, int pin3, int pin4, int home_switch)
    {
      m_pin1 = pin1;
      m_pin2 = pin2;
      m_pin3 = pin3;
      m_pin4 = pin4;
      m_zero_pin = home_switch;
      m_position = 0;
      pinMode(m_pin1, OUTPUT);
      pinMode(m_pin2, OUTPUT);
      pinMode(m_pin3, OUTPUT);
      pinMode(m_pin4, OUTPUT);
      if (m_zero_pin > 0) {
        pinMode(m_zero_pin, INPUT);
      }
      m_min_time_milli = 1.6;
      init = true;
    }
    Stepmotor() {
      // uninitialized motor object
      // use initMotor(...) to initialize
      init = false;
    }
    void initMotor(int pin1, int pin2, int pin3, int pin4, int home_switch)
    {
      m_pin1 = pin1;
      m_pin2 = pin2;
      m_pin3 = pin3;
      m_pin4 = pin4;
      m_zero_pin = home_switch;
      m_position = 0;
      pinMode(m_pin1, OUTPUT);
      pinMode(m_pin2, OUTPUT);
      pinMode(m_pin3, OUTPUT);
      pinMode(m_pin4, OUTPUT);
      if (m_zero_pin > 0) {
        pinMode(m_zero_pin, INPUT);
      }
      m_min_time_milli = 1.6;
      init = true;
    }


    // user space functions

    boolean step(boolean forward) {
      int this_step;
      // moves counter one step
      if (!init) return false;
      if (forward) {
        m_position += 1;
      } else {
        m_position -= 1;
      }
      // power the coils:
      this_step = m_position % 4;
      digitalWrite(m_pin1, coils[this_step][0]);
      digitalWrite(m_pin2, coils[this_step][1]);
      digitalWrite(m_pin3, coils[this_step][2]);
      digitalWrite(m_pin4, coils[this_step][3]);
      return true;
    }

    boolean power_off() {
      if (!init) return false;
      digitalWrite(m_pin1, LOW);
      digitalWrite(m_pin2, LOW);
      digitalWrite(m_pin3, LOW);
      digitalWrite(m_pin4, LOW);
      return true;
    }

    // If your motor has a different step wait time
    void set_min_wait(float millisecs) {
      m_min_time_milli = millisecs;
    }

    boolean home() {
      if (!init) return false;
      if (m_zero_pin > 0) {
        // rotate reverse until home switch is closed

        // TODO

        m_position = 0;
        return true;
      } else
        return false;
    }
    long getPosition() {
      return m_position;
    }
    /*
       struct motdef {
      int m_pin1 = 0;
      int m_pin2 = 0;
      int m_pin3 = 0;
      int m_pin4 = 0;
      int m_zero_pin = -1;
      unsigned long m_position = 0;
      };
    */
    motdef getMotorInfo() {
      motdef ret;
      ret.m_pin1 = m_pin1;
      ret.m_pin2 = m_pin2;
      ret.m_pin3 = m_pin3;
      ret.m_pin4 = m_pin4;
      ret.m_zero_pin = m_zero_pin;
      ret.m_position = m_position;
      return ret;
    }
};

/* A MultiMotorArray is used to move multiple stepper motors in a coordinated way

    Constructor MultiMotorArray(float millisecs)

    boolean addMotor(char motorLetter, int pin1, int pin2, int pin3, int pin4, int home_switch)
    - Adds a motor

    boolean prepMove(char motorLetter, long toPosition)
    - prepares the motion of one motor for a coordinated move

    boolean goMove(float stepTimeMilliSecs)
    - executes a prepared coordinated move

    boolean clearMove()
    - erases all preparations for a coordinated move
*/

class MultiMotorArray
{
    Stepmotor motors[maxMotors]; // the step motors object list
    char cMotors[maxMotors]; // Motor Letters for later use in command language
    unsigned long m_steps[maxMotors]; // steps in the current motion to do
    unsigned long m_steps_done[maxMotors]; // steps in current motion done
    float m_waitstep[maxMotors]; // time to wait for one step
    boolean m_forward[maxMotors]; // forward direction for next move
    int definedMotors; // number of known motors
    float m_min_time_milli;

    // Settings
    boolean autoPowerOff= true;

  public:
    MultiMotorArray(float millisecs) {
      definedMotors = 0;
      // minimum pause between two steps (depends on voltage for example!)
      // usually 1.6 millisecs are sufficient
      m_min_time_milli = millisecs;
    }
    boolean addMotor(char motorLetter, int pin1, int pin2, int pin3, int pin4, int home_switch) {
      if (definedMotors >= maxMotors) {
        return false;
      }
      motors[definedMotors].initMotor(pin1,  pin2,  pin3,  pin4,  home_switch);
      motors[definedMotors].set_min_wait(m_min_time_milli);
      cMotors[definedMotors] = motorLetter;
      definedMotors += 1;
      return true;
    }


    boolean prepMove(char motorLetter, unsigned long toPosition)
    // prepares the motion of one motor for a coordinated move
    {
      int iMotor = -1;
      // get the motor index
      for (int i = 0; i < definedMotors; i++) {
        if (cMotors[i] == motorLetter) {
          iMotor = i;
          break;
        }
      }
      if (iMotor < 0) {
        // motor letter not found
        return false;
      }
      prepMove(iMotor, toPosition);
    }
    boolean prepMove(int iMotor, unsigned long toPosition) {
      Serial.print("Motor ");
      Serial.println(iMotor);
      // calculate the required steps for this motor, if negative steps will be done with neg direction
      if (motors[iMotor].getPosition() <= toPosition) {
        // forward
        m_steps[iMotor] = toPosition - motors[iMotor].getPosition();
        m_forward[iMotor] = true;
      } else {
        // reverse
        m_forward[iMotor] = false;
        m_steps[iMotor] = motors[iMotor].getPosition() - toPosition;
      }
      return true;
    }



    boolean goMove(float stepTimeMilliSecs)
    // executes a prepared coordinated move
    {
      // calculate the speed ratings for the motors
      if (stepTimeMilliSecs < m_min_time_milli) stepTimeMilliSecs = m_min_time_milli; // respect fastes speed


      // Wie viele Schritte muss der MOtor mit den meisten Schritten fahren?
      long max_steps = 0;
      for (int i = 0; i < definedMotors; i++) {
        if (m_steps[i] > max_steps) max_steps = m_steps[i];
        m_steps_done[i] = 0;
        Serial.print(i);
        Serial.print(": Steps ");
        Serial.println(m_steps[i]);
      }
      // Wie viele Microsekunden muss der Motor bis zum nächsten Schritt warten?

      // "Nächster Schritt bei Zeit" = "Schritt#" * wait_time * max_steps/"Schritte dieser Motor"

      int m_should = 0;
      for (int i = 0; i < definedMotors; i++) {
        if (m_steps[i] > 0) {
          m_waitstep[i] = stepTimeMilliSecs * (float)max_steps / (float)m_steps[i];
          m_should++;
        }
      }

      //

      // loop with the appropriate timings

      unsigned long microsStart;
      unsigned long microsNow;
      unsigned long now; // micros since start of motion (delta)
      int m_done = 0;
      microsStart = micros(); // start value
      do {
        microsNow = micros();
        if (microsNow < microsStart) {
          // had an overflow during this motion
          now = (ulongMax - microsStart) + micros();
        } else
          now = micros() - microsStart;
        for (int i = 0; i < definedMotors; i++) {
          if (m_steps[i] > 0) {
            if (1000.0 * m_waitstep[i] * (float)(m_steps_done[i] + 1) < now) {
              // move that motor one step
              motors[i].step(m_forward[i]);
              //Serial.print(i);
              m_steps_done[i]++;
              if (m_steps_done[i] == m_steps[i]) {
                m_done++;
                Serial.print(i);
                Serial.print(" done: ");
                Serial.println(m_steps_done[i]);
                m_steps[i] = 0;
              }
            }
          }
        }
        delayMicroseconds(stepTimeMilliSecs / 2.0); // check twice in
      } while (m_done < m_should);
    }
    boolean clearMove()     // erases all preparations for a coordinated move
    {
      for (int i = 0; i < definedMotors; i++) {
        m_steps[i] = 0;
      }
    }

    boolean powerOff() { /* motor power off */
      for (int i = 0; i < definedMotors; i++) {
        motors[i].power_off();
      }
    }
    /*************************************************************************************************************/
    /*
      MD Letter pin1 pin2 pin3 pin4 [homeSwitchPin] - Motor Definition
      ML - Motor List
      GA Letter Position [Letter2 Position2 ...] [FD StepsPerSecond] - Go Absolute
      [CL - Clear Moves]
      MOFF - Motors Off
      APO 0/1 - Auto Power Off after Motion
    */
    boolean commandParser(String serialInput) {
      Serial.print("Parsing command ");
      Serial.println(serialInput);
      String cmd;
      int index = serialInput.indexOf(' ');
      float gminWait = m_min_time_milli;
      if (index > 0) {
        cmd = serialInput.substring(0, index);
      } else {
        cmd = serialInput;
      }

      if (cmd == "MD") {   // ADDING A NEW MOTOR ----------------------------------------------------------------------------------------
        // first parameter is motor letter
        if (index > (serialInput.length() - 1)) {
          // no more characters
          Serial.println("ERROR: Missing parameters for GA (Go Absolute)");
          return false;
        }
        String remainder = serialInput.substring(index + 1);
        String part;
        int pin[5];
        int homepin = -1;
        char motorletter;
        int next = remainder.indexOf(' ');
        if (next < 0) {
          // end of string reached
          next = remainder.length();
        }
        part = remainder.substring(0, next);

        int iMotor = -1;
        // get the motor index
        for (int i = 0; i < definedMotors; i++) {
          if (cMotors[i] == part[0]) {
            iMotor = i;
            break;
          }
        }
        if (iMotor >= 0) {
          // Motor is already there
          Serial.println("ERROR: Motor letter already used - Use 'ML' to get a list");
          return false;
        }
        motorletter = part[0];

        // Following should be 4 or 5 pin definitions
        // pin 1-4

        for (int pins = 1; pins <= 4; pins++) {
          if (next + 1 < remainder.length()) {
            remainder = remainder.substring(next + 1);
            next = remainder.indexOf(' ');
            if (next < 0) {
              // end of string reached
              next = remainder.length();
            }
            part = remainder.substring(0, next);
            if (part.toInt() <= 0) {
              // error
              Serial.print("ERROR: Invalid definition for coil pin ");
              Serial.println(pins);
              return false;
            }
            pin[pins] = part.toInt();
          } else {
            Serial.print("ERROR: Missing definition for coil pin ");
            Serial.println(pins);
            return false;
          }
        }
        // home pin
        if (next + 1 < remainder.length()) {
          remainder = remainder.substring(next + 1);
          next = remainder.indexOf(' ');
          if (next < 0) {
            // end of string reached
            next = remainder.length();
          }
          part = remainder.substring(0, next);
          if (part.toInt() <= 0) {
            // error
            Serial.println("ERROR: Invalid definition for home/ end stop pin ");
            return false;
          }
          homepin = part.toInt();
        } else {
          Serial.println("Info: No home pin defined pin.");
        }
        if (addMotor(motorletter, pin[1], pin[2], pin[3], pin[4], homepin)) {
          // ok
          Serial.println("OK");
        } else {
          // error
          Serial.println("ERROR: Could not add motor. Out of spares?");
        }

      } else if (cmd == "ML") {    // print motor list --------------------------------------------------------------------------------------------
        motdef motorInspect;
        for (int i = 0; i < definedMotors; i++) {
          motorInspect = motors[i].getMotorInfo();
          Serial.print("Info Motor ");
          Serial.print(i);
          Serial.print(" [");
          Serial.print(cMotors[i]);
          Serial.println("]:");
          Serial.print(" Coil Pins: ");
          Serial.print(motorInspect.m_pin1);
          Serial.print(" ");
          Serial.print(motorInspect.m_pin2);
          Serial.print(" ");
          Serial.print(motorInspect.m_pin3);
          Serial.print(" ");
          Serial.println(motorInspect.m_pin4);
          if (motorInspect.m_zero_pin >= 0) {
            Serial.print(" End stop (home) pin: ");
            Serial.println(motorInspect.m_zero_pin);
          } else {
            Serial.println(" No End stop (home) pin.");
          }
          Serial.print(" Current position: ");
          Serial.println(motorInspect.m_position);
        }
        Serial.println("OK");

      } else if (cmd == "GA") { // Go Absolute ------------------------------------------------------------------------------------------
        if (index > (serialInput.length() - 1)) {
          // no more characters
          Serial.println("ERROR: Missing parameters for GA (Go Absolute)");
          return false;
        }
        String remainder = serialInput.substring(index + 1);
        String part;
        int next = -1;
        float feedspeed;
        while (remainder.length() > 0) {
          next = remainder.indexOf(' ');
          if (next < 0) {
            // end of string reached
            next = remainder.length();
          }
          part = remainder.substring(0, next);
          if (part == "FD") {
            Serial.println("Feedrate");
            remainder = remainder.substring(next + 1);
            next = remainder.indexOf(' ');
            if (next < 0) {
              // end of string reached
              next = remainder.length();
            }
            part = remainder.substring(0, next);
            if (part.toFloat() > 0) {
              feedspeed = part.toFloat();
              Serial.print(feedspeed);
              Serial.println(" steps per second");
              // jump over the value
              gminWait = max(m_min_time_milli, 1000 / feedspeed);
              Serial.print(gminWait);
              Serial.println(" milliseconds wait time between steps");
              remainder = remainder.substring(next + 1);
            } else {
              // error
              Serial.print("ERROR: Invalid StepsPerSeconds ");
              Serial.println(part);
              return false;
            }
          } else {
            // must be a letter of a motor!
            if (part.length() > 1) {
              Serial.print("ERROR: Invalid identifier ");
              Serial.println(part);
              return false;
            }
            int iMotor = -1;
            // get the motor index
            for (int i = 0; i < definedMotors; i++) {
              if (cMotors[i] == part[0]) {
                iMotor = i;
                break;
              }
            }
            if (iMotor >= 0) {
              Serial.print("OK: motor ");
              Serial.print(part);
              Serial.print(" is motor #");
              Serial.println(iMotor);
              remainder = remainder.substring(next + 1);
              // next part must be a position value

              next = remainder.indexOf(' ');
              if (next < 0) {
                // end of string reached
                next = remainder.length();
              }
              part = remainder.substring(0, next);
              if ((part.toInt() > 0) || (part == "0")) {
                unsigned long mpos = part.toInt();
                Serial.print("OK: position ");
                Serial.println(mpos);
                prepMove(iMotor, mpos);
              } else {
                Serial.print("ERROR: unrecognized position ");
                Serial.println(part);
                return false;
              }
              // jump over the value
              remainder = remainder.substring(next + 1);
            } else
            {
              Serial.print("ERROR: unrecognized motor ");
              Serial.println(part);
              return false;
            }
          }
        }
        goMove(gminWait);
        clearMove();
        if (autoPowerOff) powerOff();
        Serial.println("OK");
        return true;
      } else if (cmd == "CL") { // CLEAR -------------------------------------------------------------------------------------------------
        clearMove();
        Serial.println("OK All move preparations cleared.");
        return true;
      } else if (cmd == "MOFF") { // Motor power off ---------------------------------------------------------------------------------------
        powerOff();
        Serial.println("OK Stepper motors power off");
        return true;
      } else if (cmd == "APO") { // Motor power off ---------------------------------------------------------------------------------------
        if (index > (serialInput.length() - 1)) {
          // no more characters
          Serial.println("ERROR: Missing parameters for APO (Auto Power Off)");
          return false;
        }
        String remainder = serialInput.substring(index + 1);
        int next = remainder.indexOf(' ');
        if (next < 0) {
          // end of string reached
          next = remainder.length();
        }
        String part = remainder.substring(0, next);
        if (part == "0") {
          autoPowerOff = false;
          Serial.println("OK Stepper motors STAY ON after each move - watch battery and temperature!");
        } else if (part == "1") {
          autoPowerOff = true;
          Serial.println("OK Stepper motors power off after each move");
        }
        return true;

      } else {
        Serial.println("ERROR unknown commend");
        return false;
      }
    }

};


/*
   Event Handler
*/
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      // add it to the inputString:
      inputString += inChar;

    }
  }
}



MultiMotorArray mots(minWait);

void setup() {

  Serial.begin(9600);
  // SAMPLE CODE ###############################################################################################
  Serial.println("Motor A");
  mots.addMotor('A', 4, 5, 6, 7, -1); // A motor at pins 4..7 with no home switch
  Serial.println("Motor B");
  mots.addMotor('B', 8, 9, 10, 11, -1);
  Serial.println("Motor C");
  mots.addMotor('C', 22, 23, 24, 25, -1);
  /*
    Serial.println("Motor A - prep 2048 steps");
    Serial.println(mots.prepMove('A', 2048));
    Serial.println("Motor B - prep 512 steps");
    Serial.println(mots.prepMove('B', 2048 / 4));
    Serial.println("Motor C - prep 128 steps");
    Serial.println(mots.prepMove('C', 128));

    //- prepares the motion of one motor for a coordinated move
    Serial.println("Wait 10 seconds for init");
    delay(10);
    Serial.println("goMove(2.0)");

    mots.goMove(4.0);
  */
  // executes a prepared coordinated move
  Serial.println("clearMove()");

  mots.clearMove();
  Serial.println("PowerOff");

  mots.powerOff();
 // /SAMPLE CODE ##############################################################################################
}

void loop() {
  // wait for event handler to recognize input (ended with NEWLINE) and pass it to the parser
  if (stringComplete) {
    Serial.println(inputString);
    mots.commandParser(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}
