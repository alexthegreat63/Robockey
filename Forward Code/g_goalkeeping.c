/* Name: r_hockey_playing.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_usb.h"
#include "m_general.h"

#include "r_motor_drive.h"
#include "r_parameters.h"
#include "r_geometry.h"
#include "g_parameters.h"
#include "r_led.h"
#include "g_motor_drive.h"

extern char buffer[PACKET_LENGTH];
extern bool packet_received;
extern bool stop_flag;
extern unsigned char score_us;
extern unsigned char score_them;

/* Wii variables */
extern unsigned int blobs[]; // 12 element buffer for Wii data
extern unsigned int starLocPrev[]; // location of previous stars
extern bool missedPoint;
extern bool missedPointPrev; // whether we were missing a point previously
extern double theta; // robot's current angle wrt line from center to goal B
extern double positionX; // robot's current x position
extern double positionY; // robot's current y position
double y_goal_high;
double y_goal_low;
double y_goal_left;
double y_goal_right;

// NOTE - These variables must be retreived from switch values at init().
extern bool towardB; // direction of robot: 1 towards B, 0 towards A
extern bool isBlue; // Indicates team color of robot.

// Assigns robot direction to go toward opposite side from starting position
void g_assignDirection() {
  if(check(PINF,G_COLOR_IN)) {
    towardB = true;
    blueOn();
    // assign high and low side of goal
    y_goal_high = Y_GOAL_RED_HIGH;
    y_goal_low = Y_GOAL_RED_LOW;
  } else {
    towardB = false;
    redOn();
    y_goal_high = Y_GOAL_BLUE_HIGH;
    y_goal_low = Y_GOAL_BLUE_LOW;
  }
  //y_goal_center = (y_goal_low+y_goal_high)/2;

  // Assign right and left directions for goalie based on what goal we're in
  if(positionX >= 0) {
    y_goal_left = y_goal_high;
    y_goal_right = y_goal_low;
  } else {
    y_goal_left = y_goal_low;
    y_goal_right = y_goal_high;
  }
}

// Seek puck
void g_puckFind() {
  if(!check(PIND, G_FRONT_SENSOR)) { // if puck is directly ahead
    g_driveMotor(true,1); // stop motors
  } else if(!check(PIND,G_LEFT_SENSOR)) { // if puck is to left
    if(positionY >= y_goal_high || positionY <= y_goal_low) {
      g_driveMotor(true,1); // stop motors
    } else {
      g_driveMotor(false, 0xFFFF/4); // move left
    }
  } else if(!check(PIND,G_RIGHT_SENSOR)) { // if puck is to right
    if(positionY >= y_goal_high || positionY <= y_goal_low) {
      g_driveMotor(true,1); // stop motors
    } else {
      g_driveMotor(true, 0xFFFF/4); // move right
    }
  } else { // default state: stop
    g_driveMotor(true,1);
  }
}