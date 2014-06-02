/*
# FrisBot v0.1
#
# Copyright 2014 T.Schaller
# All rights reserved.
# All code released under the GPLv2 license.
#
# --------------------------------------------
# frisbot.h
# --------------------------------------------
*/

// -------------------------------------------
// Define common values
// -------------------------------------------
#define PROGNAME "FrisBot"
#define VERSION "v0.01"

#define FALSE 0
#define TRUE 1

//#define SERIAL_BPS 9600
#define SERIAL_BPS 57600

#define DEBUG_MIN 1
#define DEBUG_2 2
#define DEBUG_3 3
#define DEBUG_4 4
#define DEBUG_5 5
#define DEBUG_6 6
#define DEBUG_7 7
#define DEBUG_8 8
#define DEBUG_9 9
#define DEBUG_MAX 10

// directions
#define STOP 0
#define SPEED_MID 127
#define SPEED_MAX 255

#define FORWARD 0
#define REVERSE 1
#define LEFT    2
#define RIGHT   3

// -------------------------------------------
// Create the frisbot_cmd struct and all
// command queue manipulation commands.
// -------------------------------------------
#define MAX_CMD_QUEUE 1024

// 4 counts = 1 wheel revolution
#define COUNT_PER_UNIT 4

// 1000 millis = 1 second. Used with CMD_PAUSE
#define MILLIS_PER_UNIT 100

#define CMD_NOOP 0
#define CMD_FORWARD 1
#define CMD_REVERSE 2
#define CMD_CCW 3
#define CMD_CW 4
#define CMD_FWD_CCW 5
#define CMD_FWD_CW 6
#define CMD_REV_CCW 7
#define CMD_REV_CW 8
#define CMD_ROT_CCW 9
#define CMD_ROT_CW 10

#define CMD_SET_SPEED 11
#define CMD_PAUSE 12
#define CMD_STOP 13

#define CMD_FIRE 14
#define CMD_LABEL 15
#define CMD_GOTO 16

char* cmd_name[] =  { "NOOP", "FORWARD", "REVERSE", "CCW", "CW", "FWD_CCW", "FWD_CW",
                      "REV_CCW", "REV_CW", "ROT_CCW", "ROT_CW", "SET_SPEED", "PAUSE",
                      "STOP", "FIRE", "LABEL", "GOTO" };

struct frisbot_cmd {
  int command;
  int value;
};

frisbot_cmd cmd_queue[MAX_CMD_QUEUE];
  
void load_demo() {
  if( DEBUG > DEBUG_MIN ) {
    Serial.println( "load_demo()" );
  }
  
  // Zero out the command queue
  for( int x = 0; x <= MAX_CMD_QUEUE; x++) {
    cmd_queue[x].command = CMD_STOP;
    cmd_queue[x].value = 0;
  }

  // Demo command_queue
  cmd_queue[0].command = CMD_SET_SPEED;
  cmd_queue[0].value   = 64;
  
  cmd_queue[1].command = CMD_REVERSE;
  cmd_queue[1].value   = 100;
  
  cmd_queue[2].command = CMD_CCW;
  cmd_queue[2].value   = 10;
  
  cmd_queue[3].command = CMD_CW;
  cmd_queue[3].value   = 10;
  
  cmd_queue[4].command = CMD_FORWARD;
  cmd_queue[4].value   = 100;
  
  cmd_queue[5].command = CMD_SET_SPEED;
  cmd_queue[5].value   = 0;

  if( DEBUG > DEBUG_MIN ) {
    Serial.println( "------------------------------" );
  }
}


