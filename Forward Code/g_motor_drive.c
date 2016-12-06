/* Name: r_rf.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_general.h"
#include "r_parameters.h"
#include "g_parameters.h"

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

extern bool left_stopped;
extern bool right_stopped;
extern bool g_stopped;

/*
 * Drive the left motor forward
 * Inputs: 
 *  direction (1 forward, 0 backward)
 *  duty (0xFFFF = 100% duty, 0 = 0% duty)
 */
/*
 * Drive the motors right/left
 * Inputs: 
 *  direction (1 right, 0 left)
 *  duty (0xFFFF = 100% duty, 0 = 0% duty)
 */
void g_driveMotor(bool direction_right, unsigned int duty) {
  if(direction_right) {
    set(PORTB,RIGHT_DIRECTION);
  } else {
    clear(PORTB,RIGHT_DIRECTION);
  }
  OCR1C = duty;
}

void g_stop() {
  g_stopped = true;
}