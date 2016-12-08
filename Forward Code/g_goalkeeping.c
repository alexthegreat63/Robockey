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
  if(check(PINF,LED_IN)) {
    towardB = true;
    redOn();
    // assign high and low side of goal
    y_goal_high = G_Y_GOAL_RED_HIGH;
    y_goal_low = G_Y_GOAL_RED_LOW;
  } else {
    towardB = false;
    blueOn();
    y_goal_high = G_Y_GOAL_BLUE_HIGH;
    y_goal_low = G_Y_GOAL_BLUE_LOW;
  }
}

// Seek puck
void g_puckFind() {
  if(!check(PIND, G_FRONT_SENSOR)) { // if puck is directly ahead
    g_stop(); // stop motors
    m_red(ON);
    m_green(ON);
  } else if(!check(PIND,G_LEFT_SENSOR)) { // if puck is to left
    m_red(OFF);
    m_green(ON);
    if(towardB) {
       if(positionY <= G_Y_GOAL_RED_HIGH) { // if we are in 
        g_driveMotor(true); // drive motor left
       }
    } else {
      if(positionY >= G_Y_GOAL_BLUE_LOW) {
        g_driveMotor(true);
      }
    }
  } else if(!check(PIND,G_RIGHT_SENSOR)) { // if puck is to right
        m_red(ON);
    m_green(OFF);
    if(towardB) {
       if(positionY >= G_Y_GOAL_RED_LOW) { // if we are in 
        g_driveMotor(false); // drive motor left
       }
    } else {
      if(positionY <= G_Y_GOAL_BLUE_HIGH) {
        g_driveMotor(false);
      }
    }
  } else { // default state: stop
    m_red(OFF);
    m_green(OFF);
    // keep doing what we were doing as long as we're in bounds
  }
}