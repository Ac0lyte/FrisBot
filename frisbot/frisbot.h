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

#define FALSE 0
#define TRUE 1

// -------------------------------------------
// Create the frisbot_cmd struct and all
// command queue manipulation commands.
// -------------------------------------------

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


