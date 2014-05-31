/*
# FrisBot v0.1
#
# Copyright 2014 T.Schaller
# All rights reserved.
# All code released under the GPLv2 license.
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

// Interrupt settings
int pin_left_hall = 2;
int int_left_hall = 0;

int pin_right_hall = 3;
int int_right_hall = 1;

volatile unsigned int left_count = 0;
volatile unsigned int right_count = 0;

// command queue
#define MAX_CMD_QUEUE 1024

int cmd_queue[MAX_CMD_QUEUE];
int cmd_data[MAX_CMD_QUEUE];

int cmd_active = 0;
int cmd_idx = 0;

// directions
#define FORWARD 0
#define REVERSE 1
#define LEFT    2
#define RIGHT   3

// states
#define STOP 0

// motor control
int left_dir = FORWARD;
int right_dir = FORWARD;

int left_speed = STOP;
int right_speed = STOP;

// targets
int left_target = 0;
int right_target = 0;

void setup()
{
  // Attach the hall sensors interrupt routines.
  attachInterrupt(int_left_hall, left_hall, RISING);
  digitalWrite(pin_left_hall, HIGH);

  attachInterrupt(int_left_hall, left_hall, RISING);
  digitalWrite(pin_left_hall, HIGH);
}

void loop()
{
  if( cmd_active > 0 ) {
    if( at_targets() ) {
      cmd_active = 0;
    }
    set_motors();
  } else {
    cmd_idx++;
    cmd_active++;

    if( cmd_idx < MAX_CMD_QUEUE ) {
      set_targets();
    }
  }
  
  
}

// Inttrupts for hall counters
void left_hall()
{
  left_count++;
}

void right_hall()
{
  right_count++;
}


// command targets are where we would like
// to see things move to, for example counters.

void set_targets() {
  ;
}

int at_targets() {
  return 0;
}

// Speed control, speed contron, speed control, stylin'

void set_motors() {
  ;
}
