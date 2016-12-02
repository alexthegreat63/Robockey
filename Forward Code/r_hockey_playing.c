/* Name: r_puck_sense.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_general.h"
#include "r_motor_drive.h"
#include "r_parameters.h"

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

// Causes robot to go directly to desired goal
void goToGoal() {
  int y_high;
  int y_low;
  int x;
  if(towardB) {
    y_high = Y_GOAL_B_HIGH;
    y_low = Y_GOAL_B_LOW;
    x = X_GOAL_B;
  } else {
    y_high = Y_GOAL_A_HIGH;
    y_low = Y_GOAL_A_LOW;
    x = X_GOAL_A;
  }
  double theta_goal_high = -atan2(y_high-positionY,x-positionX); // angle to high side of the goal
  double theta_goal_low = -atan2(y_low-positionY,x-positionX); // angle to low side of the goal
  if(theta > 3.14159) {
    theta -= 2*3.14159;
  } else if (theta < -3.14159) {
    theta+= 2*3.14159;
  }
  if(theta-theta_goal_high<0) { // rotate right
    m_green(OFF);
    driveRightMotor(true,0xFFFF/4);
    driveLeftMotor(false,0xFFFF/4);
  } else if(theta-theta_goal_low>0) { // rotate left
    m_green(OFF);
    driveRightMotor(false,0xFFFF/4);
    driveLeftMotor(true,0xFFFF/4);
  } else {
    m_green(ON);
    driveLeftMotor(true,0xFFFF/4);
    driveRightMotor(true,0xFFFF/4);
  }
}