// 6-11-2024
// by: Sybren Postma
// focuser.ino
// Main program for the moonlite-based focuser.
// Tested using a SeeedStudio XIAO RP2040 and A4988 stepper motor driver.
// Uses Moonlite library from Hansastro: https://github.com/Hansastro/Focuser
// Project GitHub: https://github.com/GeneralSyb/Step-moonlite-astrofocus

float speed = 10000; // Maximum speed
float acceleration = 500; // Acceleration, in this case the same for all speeds

// Required libraries
#include <Moonlite.h> // Focuser driver
#include <AccelStepper.h> // Library to handle acceleration and decceleration using STP/DIR pins

// Pin definitions. Comment includes the Seeeduino XIAO pin name
// If other pins are used because you use a different driver, change the program accordingly.
// For example changing the amount of MS pins, remove or add #define MSn {pin} here, change the microstepping settings in the processCommand() function accordingly.
// Also change the code in setup() accordingly.
#define stp 27    // D1 - STEP pin
#define dir 26    // D0 - Direction pin
#define MS1 7     // D5 - Microstep 1 pin
#define MS2 6     // D4 - Microstep 2 pin
#define MS3 29    // D3 - Microstep 3 pin
#define led 17    // Builtin LED, G:16, R:17, B:25
#define en 2      // D8 - Driver enable
#define rst 4     // D9 - Driver reset
#define slp 28    // D2 - Driver sleep

AccelStepper stepper(1, stp, dir); // Stepper motor object

Moonlite SerialProtocol; // Moonlite object

long targetPosition = 0; // Target position
bool halfStep = false; // Microstepping mode, full- and halfstep doesn't necessarily mean it is doing 1/1 and 1/2 microstepping. It is more just the scale.
float tempCo = 1; // Temperature compensation coefficient, not used.
float compVal = 1; // Temperature value, not used.
bool tempCompEn = false; // Temperature compensation enable, not used.

void processCommand()
{
  MoonliteCommand_t command;
  switch (SerialProtocol.getCommand().commandID)
  {
    case ML_C:
      // Initiate temperature convertion
      // Not implemented
      break;
    case ML_FG:
      // Goto target position
      stepper.moveTo(targetPosition);
      break;
    case ML_FQ:
      // Motor stop movement
      stepper.moveTo(stepper.currentPosition()); // Cancel motor movement
      stepper.stop(); // For good measure
      break;
    case ML_GB:
      // Set the Red Led backligth value
      // Dump value necessary to run the official moonlite software
      SerialProtocol.setAnswer(2, 0x00);
    case ML_GC:
      // Return the temperature coefficient
      SerialProtocol.setAnswer(2, (long)20);
      break;
    case ML_GD:
      // Return the current motor speed
      switch ((int)speed)
      {
        case 2000:
          SerialProtocol.setAnswer(2, (long)20);
          break;
        case 3500:
          SerialProtocol.setAnswer(2, (long)10);
          break;
        case 5000:
          SerialProtocol.setAnswer(2, (long)8);
          break;
        case 7500:
          SerialProtocol.setAnswer(2, (long)4);
          break;
        case 10000:
          SerialProtocol.setAnswer(2, (long)2);
          break;
        default:
          break;
      }
      break;
    case ML_GH:
      // Return the current stepping mode (half or full step)
      SerialProtocol.setAnswer(2, (long)(halfStep ? 0xFF : 0x00));
      break;
    case ML_GI:
      // get if the motor is moving or not
      SerialProtocol.setAnswer(2, (long)(stepper.distanceToGo() != 0 ? 0x01 : 0x00));
      break;
    case ML_GN:
      // Get the target position
      SerialProtocol.setAnswer(4, targetPosition);
      break;
    case ML_GP:
      // Return the current position
      SerialProtocol.setAnswer(4, stepper.currentPosition());
      break;
    case ML_GT:
      // Return the temperature
      SerialProtocol.setAnswer(4, (long)30);
      break;
    case ML_GV:
      // Get the version of the firmware
      SerialProtocol.setAnswer(2, (long)(0x01));
      break;
    case ML_SC:
      // Set the temperature coefficient
      tempCo = SerialProtocol.getCommand().parameter;
      break;
    case ML_SD:
    
      // Set the motor speed
      switch (SerialProtocol.getCommand().parameter)
      {
        case 0x02:
          stepper.setMaxSpeed(10000);
          break;
        case 0x04:
          stepper.setMaxSpeed(7500);
          break;
        case 0x08:
          stepper.setMaxSpeed(5000);
          break;
        case 0x10:
          stepper.setMaxSpeed(3500);
          break;
        case 0x20:
          stepper.setMaxSpeed(2000);
          break;
        default:
          break;
      }
      break;
    case ML_SF:
      // Set the stepping mode to full step
      // In this case 1/8th step
      digitalWrite(MS1,HIGH);
      digitalWrite(MS2,HIGH);
      digitalWrite(MS3,LOW);
      halfStep = false;
      break;
    case ML_SH:
      // Set the stepping mode to half step
      // In this case to 1/16th step
      digitalWrite(MS1,HIGH);
      digitalWrite(MS2,HIGH);
      digitalWrite(MS3,HIGH);
      halfStep = true;
      break;
    case ML_SN:
      // Set the target position
      targetPosition = SerialProtocol.getCommand().parameter;
      break;
    case ML_SP:
      // Set the current motor position
      stepper.setCurrentPosition(SerialProtocol.getCommand().parameter);
      break;
    case ML_PLUS:
      // Activate temperature compensation focusing
      tempCompEn = true;
      break;
    case ML_MINUS:
      // Disable temperature compensation focusing
      tempCompEn = false;
      break;
    case ML_PO:
      // Temperature calibration
      compVal = SerialProtocol.getCommand().parameter / 2.0;
      break;
    default:
      break;
  }
}

void setup(){
  SerialProtocol.init(9600); // Serial setup at 9600 baud

  // Setting pins as outputs
  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);

  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);

  pinMode(slp, OUTPUT);
  pinMode(rst, OUTPUT);
  pinMode(en, OUTPUT);

  pinMode(led, OUTPUT);

  // Setting initial pin states
  digitalWrite(en,LOW);
  digitalWrite(slp,HIGH);
  digitalWrite(rst,HIGH);

  digitalWrite(MS1,HIGH);
  digitalWrite(MS2,HIGH);
  digitalWrite(MS3,HIGH);

  // Setting initial motor speed and acceleration
  stepper.setSpeed(speed);
  stepper.setAcceleration(acceleration);

  // Setting the driver enable pin
  stepper.setEnablePin(slp); // Best to use sleep and not enable
  // stepper.enableInvert(); // If the function needs inverting, uncomment this
}

void loop(){
  // Handle moonlite commands
  SerialProtocol.Manage();
  if (SerialProtocol.isNewCommandAvailable())
  {
    processCommand();
    stepper.setSpeed(speed); // Update stepper max speed.
  }
  
  // Run the stepper, if it needs stepping
  stepper.run();
}
