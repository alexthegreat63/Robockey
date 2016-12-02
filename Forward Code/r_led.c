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

 // output pins for blue/red LEDs. All Port F.
#define BLUE 5
#define RED 6
#define LED_IN 7

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

void redOn() {
  clear(PORTF,BLUE);
  set(PORTF,RED);
}

void blueOn() {
  clear(PORTF,RED);
  set(PORTF,BLUE);
}

void ledOff() {
  clear(PORTF,BLUE);
  clear(PORTF,RED);
}