/*
# FrisBot v0.1
#
# Copyright 2014 T.Schaller
# All rights reserved.
# All code released under the GPLv2 license.
#
# --------------------------------------------
# frisbot.ino
# --------------------------------------------
#
# A frisbee based big trak clone want-to-be
#
# Arduino Mega2560 + Motor Shield
#
# Two motors mounted under a frisbee. Ripped from R/C cars,
# Including gearboxes. These are connected via the shield.
#
# Hall effect sensors (one per motor)
# Using the 3144. A pull up resister is needed,
# The internal SHOULD work. YMMV
#
# Two 4x4 keypads
#
*/

#include <Keypad.h>

// --------------------------------------------
// Set debug to anything other the zero for debug output on the serial console
#define DEBUG 11

#include "frisbot.h"

// --------------------------------------------
// Interrupts used
// http://arduino.cc/en/Reference/attachInterrupt

int int_right_hall = 2;  // Mega2560 pin 21
int int_left_hall = 3;   // Mega2560 pin 20

// --------------------------------------------
// Pins used
int pin_left_speed = 3;
int pin_right_break = 8;
int pin_left_break = 9;
int pin_right_speed = 11;
int pin_left_direction = 13;
int pin_right_direction = 13;
int pin_left_hall = 20;
int pin_right_hall = 21;

// keyboards 22-37
int pin_kb1_c1 = 22;
int pin_kb1_c2 = 23;
int pin_kb1_c3 = 24;
int pin_kb1_c4 = 25;
int pin_kb1_r1 = 26;
int pin_kb1_r2 = 27;
int pin_kb1_r3 = 28;
int pin_kb1_r4 = 29;

int pin_kb2_c1 = 30;
int pin_kb2_c2 = 31;
int pin_kb2_c3 = 32;
int pin_kb2_c4 = 33;
int pin_kb2_r1 = 34;
int pin_kb2_r2 = 35;
int pin_kb2_r3 = 36;
int pin_kb2_r4 = 37;


// --------------------------------------------
// wheel turn counters
volatile unsigned int left_count = 0;
volatile unsigned int right_count = 0;

// --------------------------------------------
// command queue
extern frisbot_cmd cmd_queue[MAX_CMD_QUEUE];

int cmd_active = FALSE;
int cmd_index = -1;

// --------------------------------------------
// motor control

int left_dir = FORWARD;
int right_dir = FORWARD;

int left_speed = STOP;
int right_speed = STOP;
int current_speed = STOP;


// --------------------------------------------
// targets
unsigned int left_target = 0;
unsigned int right_target = 0;
unsigned int pause_target = 0;

unsigned int left_last = 0;
unsigned int right_last = 0;

// --------------------------------------------
// keyboard setup

const byte kb_rows = 4; //four rows
const byte kb_cols = 4; //three columns
char kb_keys[kb_rows][kb_cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'#','0','*','D'}
};
char kb2_keys[kb_rows][kb_cols] = {
  {'q','w','e','r'},
  {'a','s','d','f'},
  {'z','x','c','v'},
  {'Q','W','E','R'}
};

byte kb1_rowPins[kb_rows] = {pin_kb1_r1, pin_kb1_r2, pin_kb1_r3, pin_kb1_r4};
byte kb1_colPins[kb_cols] = {pin_kb1_c1, pin_kb1_c2, pin_kb1_c3, pin_kb1_c4};
Keypad keypad1 = Keypad( makeKeymap(kb_keys), kb1_rowPins, kb1_colPins, kb_rows, kb_cols );

byte kb2_rowPins[kb_rows] = {pin_kb2_r1, pin_kb2_r2, pin_kb2_r3, pin_kb2_r4};
byte kb2_colPins[kb_cols] = {pin_kb2_c1, pin_kb2_c2, pin_kb2_c3, pin_kb2_c4};
Keypad keypad2 = Keypad( makeKeymap(kb2_keys), kb2_rowPins, kb2_colPins, kb_rows, kb_cols );


// --------------------------------------------
void setup()
{
  // Show a banner if debugging
  if( DEBUG > DEBUG_MIN ) {
    Serial.begin(SERIAL_BPS);
    Serial.print( PROGNAME );
    Serial.print( " " );
    Serial.println( VERSION );
  }

  // Attach the hall sensors interrupt routines.
  attachInterrupt(int_left_hall, left_hall, RISING);
  digitalWrite(pin_left_hall, HIGH);

  attachInterrupt(int_right_hall, right_hall, RISING);
  digitalWrite(pin_right_hall, HIGH);

  // Break the motors
  pinMode(pin_left_break, OUTPUT);
  pinMode(pin_right_break, OUTPUT);  
  digitalWrite(pin_left_break, LOW);
  digitalWrite(pin_right_break, LOW);
  
  // Stop the motors
  pinMode(pin_left_speed, OUTPUT);
  pinMode(pin_right_speed, OUTPUT);  
  analogWrite(pin_left_speed, STOP);
  analogWrite(pin_right_speed, STOP);
  
  // Zero out the command queue
  for( int x = 0; x <= MAX_CMD_QUEUE; x++) {
    cmd_queue[x].command = CMD_STOP;
    cmd_queue[x].value = 0;
  }

  // indicate setup is finished if debugging.
  if( DEBUG > DEBUG_MIN ) {
    Serial.println( "------------------------------" );
  }
}

// --------------------------------------------
void loop()
{
  if( cmd_active == TRUE ) {
    if( at_targets() ) {
      cmd_active = FALSE;
    }
  } else {
    if( cmd_index <= MAX_CMD_QUEUE ) {
      cmd_index++;      
      cmd_active = TRUE;
      set_targets();
      set_motor();
    }
  }
  
  char key1 = keypad1.getKey();
  char key2 = keypad2.getKey();

  if (key1 != NO_KEY){
    handle_keypress(key1);
  }
  
  if (key2 != NO_KEY){
    handle_keypress(key2);
  }
  
  if( DEBUG > DEBUG_MIN ) {
    if( (left_count != left_last ) || (right_count != right_last) ){
      left_last = left_count;
      right_last = right_count;
      
      Serial.print("LEFT: ");
      Serial.print(left_count);
      Serial.print(" RIGHT: ");
      Serial.print(right_count);
      Serial.println();
    }
  }
}

// --------------------------------------------
// Inttrupts for hall counters
// --------------------------------------------

void left_hall()
{
  left_count++;
}

void right_hall()
{
  right_count++;
}

// --------------------------------------------
// command targets are where we would like
// to see things move to, for example counters.
// --------------------------------------------

void set_targets() {
  if( (DEBUG > DEBUG_MIN) && (cmd_queue[cmd_index].command != CMD_STOP) ) {
      Serial.print("COMMAND: ");
      Serial.print(cmd_name[cmd_queue[cmd_index].command]);
      Serial.print(" VALUE: ");
      Serial.print(cmd_queue[cmd_index].value);
      Serial.println();
    }
  
  // Set targets, speeds, and direction
  switch( cmd_queue[cmd_index].command ){
    case CMD_STOP:
      digitalWrite(pin_left_break, LOW);
      digitalWrite(pin_right_break, LOW);
      analogWrite(pin_left_speed, STOP);
      analogWrite(pin_right_speed, STOP);
      break;

    case CMD_FORWARD:
      left_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      right_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      left_speed = current_speed;
      right_speed = current_speed;
      left_dir = FORWARD;
      right_dir = FORWARD;
      break;

    case CMD_REVERSE:
      left_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      right_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      left_speed = current_speed;
      right_speed = current_speed;
      left_dir = REVERSE;
      right_dir = REVERSE;
      break;

    case CMD_CCW:
      left_target = left_count;
      right_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      left_speed = 0;
      right_speed = current_speed;
      left_dir = STOP;
      right_dir = FORWARD;
      break;

    case CMD_FWD_CCW:
      left_target = left_count;
      right_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      left_speed = current_speed / 2;
      right_speed = current_speed;
      left_dir = FORWARD;
      right_dir = FORWARD;
      break;

    case CMD_REV_CCW:
      left_target = left_count;
      right_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      left_speed = current_speed / 2;
      right_speed = current_speed;
      left_dir = REVERSE;
      right_dir = REVERSE;
      break;

    case CMD_ROT_CCW:
      left_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      right_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      left_speed = current_speed;
      right_speed = current_speed;
      left_dir = REVERSE;
      right_dir = FORWARD;
      break;

    case CMD_CW:
      left_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      right_target = right_count;
      left_speed = current_speed;
      right_speed = 0;
      left_dir = FORWARD;
      right_dir = STOP;
      break;

    case CMD_FWD_CW:
      left_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      right_target = right_count;
      left_speed = current_speed;
      right_speed = current_speed / 2;
      left_dir = FORWARD;
      right_dir = FORWARD;
      break;

    case CMD_REV_CW:
      left_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      right_target = right_count;
      left_speed = current_speed;
      right_speed = current_speed / 2;
      left_dir = REVERSE;
      right_dir = REVERSE;
      break;

    case CMD_ROT_CW:
      left_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      right_target += (cmd_queue[cmd_index].value * COUNT_PER_UNIT);
      left_speed = current_speed;
      right_speed = current_speed;
      left_dir = FORWARD;
      right_dir = REVERSE;
      break;

    case CMD_PAUSE:
      current_speed = STOP;
      pause_target = millis() + cmd_queue[cmd_index].value * MILLIS_PER_UNIT;
      left_speed = 0;
      right_speed = 0;
      left_dir = STOP;
      right_dir = STOP;
      break;

    case CMD_SET_SPEED:
      current_speed = cmd_queue[cmd_index].value;
      left_speed = current_speed;
      right_speed = current_speed;
      break;
  }
}

// --------------------------------------------
// Check to see if we have arrived.
//
// First check to see if we are PAUSEing.
//
// Next check to see if both motors are at 
// out destination.
//
// Next check each motor individually.
//
// If any destinations are matched then stop 
// the matched motor.
// --------------------------------------------

int at_targets() {
  if(cmd_queue[cmd_index].command == CMD_SET_SPEED) {
    return TRUE;
  }
  
  if(cmd_queue[cmd_index].command == CMD_PAUSE) {
    if( millis() >= pause_target)
      return TRUE;
  } else {
    if(( left_count >= left_target ) && ( right_count >= right_target )) {
      left_speed = STOP;
      right_speed = STOP;
      set_motor();
      return TRUE;
    }
  }

  if( left_count >= left_target ) {
    if( left_speed > STOP ) {
      left_speed = STOP;
      set_motor();
    }
  }
  
  if( right_count >= right_target ) {
    if( right_speed > STOP ) {
      right_speed = STOP;
      set_motor();
    }
  }
  
  return FALSE;
}

// --------------------------------------------
// Speed control, speed control, speed control, stylin'
// --------------------------------------------

void set_motor() {
  if( (DEBUG > DEBUG_MIN) && (cmd_queue[cmd_index].command != CMD_STOP) ) {
    Serial.print( "L.DIR ");
    Serial.print( left_dir );
    Serial.print( " L.speed ");
    Serial.print( left_speed );
    Serial.print( " L.target ");
    Serial.print( left_target );
    Serial.print( " R.DIR ");
    Serial.print( right_dir );
    Serial.print( " R.speed ");
    Serial.print( right_speed );
    Serial.print( " R.target ");
    Serial.print( right_target );
    Serial.print( "\n" );
  }

  digitalWrite(pin_left_direction, left_dir);
  digitalWrite(pin_right_direction, right_dir);

  analogWrite(pin_left_speed, left_speed);
  analogWrite(pin_right_speed, right_speed);
}

// --------------------------------------------
void handle_keypress( char key ) {
  switch( key ) {
    case 'R': 
      load_demo();
      cmd_index = MAX_CMD_QUEUE + 1;
      break;
      
    case 'Q': // Start program execution
      cmd_index = -1;
      break;
  }
  
  Serial.print(key);
}
