/* Name: r_puck_sense.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_general.h"
#include "r_parameters.h"
#include "r_hockey_playing.h"
#include "r_motor_drive.h"

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

// NOTE - These variables must be retreived from switch values at init().
extern bool towardB; // direction of robot: 1 towards B, 0 towards A
extern bool isBlue; // Indicates team color of robot.


// Seek puck
void puckFind() {
  if(!check(PIND, PUCK_LEFT_SENSOR) && !check(PIND, PUCK_RIGHT_SENSOR)) { // if puck is in our posession
    goToGoal();
  } else if(!check(PIND, FRONT_LEFT_SENSOR) && !check(PIND, FRONT_RIGHT_SENSOR)) { // if puck is directly ahead
    driveLeftMotor(true, MOTOR_SPEED);
    driveRightMotor(true, MOTOR_SPEED);
  } else if(!check(PIND,LEFT_SENSOR)) { // if puck is to left
    driveLeftMotor(false, MOTOR_SPEED);
    driveRightMotor(true, MOTOR_SPEED);
  } else if(!check(PIND,RIGHT_SENSOR)) { // if puck is to right
    driveLeftMotor(true, MOTOR_SPEED);
    driveRightMotor(false, MOTOR_SPEED);
  }
  // NOTE: May have to change the order of the if statements. Also, may call goToGoal() from here if PUCK_SENSOR is high.
}