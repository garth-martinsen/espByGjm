#include <Arduino.h>
//#include <bitset>
#include <iostream>
#include<set>



/*This project was named after bitset, but what resulted does not use bitset but uses #define integers and arrays.  I could not Serial.print a bitset so I desisted from using bitset. Instead I use state variable using 4 bits : byte position(open, close, motion, anchored). TruthTable is in doc: 
/Users/garth/Library/Mobile Documents/com~apple~TextEdit/Documents/esp32_TMC2209_StepperMotor.rtfd */

// put function declarations here:
void followCmd(int);
void showCmd(int);
void showState();
void stepperLift();
void stepperLower();
void dcMotorFWD();
void dcMotorRVS();
void dcMotorStop();

// globals

// commands
#define OPEN 1
#define CLOSE 2
#define STOP 3
#define STOP_ANCHOR 4

/* new states using bits ocma, O: OPEN  C:CLOSE  M:MOTION A: ANCHORED
TruthTable at /Users/garth/Library/Mobile Documents/com~apple~TextEdit/Documents/esp32_TMC2209_StepperMotor.rtfd */
//STATE                                OCMA
#define  STOPPED_LIFTED          0  // 0000
#define  STOPPED_ANCHORED        1  // 0001 
#define  CLOSED_LIFTED           4  // 0100
#define  CLOSED_ANCHORED         5  // 0101
#define  CLOSING                 6  // 0110
#define  OPEN_LIFTED             8  // 1000  
#define  OPEN_ANCHORED           9  // 1001
#define  OPENING                 10 // 1010


char const *const cmds[5] = {"", "OPEN", "CLOSE", "STOP", "STOP_ANCHOR"};
volatile int cmd = 0;
int state = CLOSED_ANCHORED;
std::set<int> anchors{1,5,9};  //states in which the droprods are anchored.

//LISTENING AND CONNECTED ARE NOT IN THE FOLLOWING STATES.
char const *const states[11] = {
    "STOPPED_LIFTED", "STOPPED_ANCHORED", "ILLEGAL", "ILLEGAL",
    "CLOSED_LIFTED", "CLOSED_ANCHORED", "CLOSING", "ILLEGAL",
    "OPENED_LIFTED", "OPENED_ANCHORED", "OPENING" };


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting BitSet demo...");
  showState();
  followCmd(OPEN);
  delay(10);
  followCmd(CLOSE);
  delay(10);
  state=OPENING;
  followCmd(STOP);
  state=CLOSING;
  followCmd(STOP);
  state=CLOSING;
  followCmd(STOP_ANCHOR);
  state=OPENING;
  followCmd(STOP_ANCHOR);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

// put function definitions here:
void showCmd(int ndx)
{
  /*Displays the name at index: ndx eg: 1,2,3*/
  Serial.printf("COMMAND: %s\n", cmds[ndx]);
}

void showState()
{
 /*Displays the name at index: ndx */
  Serial.printf("     state: %d:%s\n", state, states[state]);
}

void followCmd(int cmd)
{
  showCmd(cmd);
  showState();

  switch (cmd)
  {
  case OPEN:
    if (anchors.count(state))  //if not found =>0 or contained ==false
    {
      stepperLift();
    }
    dcMotorFWD();
    stepperLower();
    break;
  case CLOSE:
    if (anchors.count(state)){
      stepperLift();
    }
    dcMotorRVS();
    stepperLower();
    break;
  case STOP:
    dcMotorStop();
    break;
  case STOP_ANCHOR:
    dcMotorStop();
    stepperLower();
  }

}  
void stepperLift()
{
  ulong amt = 2222; // millisec
  ulong start = millis();

  while (millis() < start + amt)
  {
    // wait for stepper to finish.
  }
  state=state - 1;  // raised to ?_LIFTED
  showState();
 }

void stepperLower()
{
  ulong amt = 2222; // millisec
  ulong start = millis();
 
  while (millis() < start + amt)
  {
    // wait for stepper to finish.
  }
  state = state + 1; // transition to ?_ANCHORED
 
  showState();
 
}

void dcMotorFWD()
{
  /*dcMotor will continue until limit switch is hit or STOP cmd is given. State will then be changed. Here we will simulate motor action with a 15 second wait for opening to complete. Later we will use an interupt to execute a STOP. */
  ulong wait = 15000;
  ulong start = millis();
  state=OPENING; // 
  showState();
  while (millis() < start + wait)
  {
    // motor is running
  }
  state = state -2;   //transition to ?_LIFTED=8
  showState();
 
}

void dcMotorRVS()
{
  /*dcMotor will continue until limit switch is hit or a STOP cmd is given.
State will then be changed. Here we will simulate a complte closing by waiting
15 seconds for closing to complete. Later an interrupt will be used to execute a STOP. */
  ulong wait = 15000;
  ulong start = millis();
  state = CLOSING; // zero based so subtract 1 from desired state.
  showState();
  while (millis() < start + wait)
  {
    // motor is running in reverse which closes gate.
  }
  state=state-2;
  showState();
}

void dcMotorStop()
{
  /*Used for cases when the gate should not continue to complete an OPEN or a CLOSE command due to safety concerns.  The STOP command will be tied to an interrupt. When the interrupt is called, it will stop the dcMotor. A STOPPED state can then be followed by an OPEN, or a CLOSE command. More TBD...*/
  state=STOPPED_LIFTED;
  showState();
}
