/* Name: r_hockey_playing.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_general.h"
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

double angleRange(double th) {
  double r_th;
  if (th > 3.14159) {
    r_th = th-6.2831853;
  } else if(th < -3.14159) {
    r_th = th+6.2831853;
  } else {
    r_th = th;
  }
  return r_th;
}

// finds if thCheck is in smallest range of th1 and th2
// returns true if it is, false if it isn't
bool wrapCompare(double thCheck, double th1, double th2) {
  bool inRange;
  if(angleRange(th1-th2)<M_PI && angleRange(th1-th2)>=0) {
    if(angleRange(th1-thCheck) > 0 && angleRange(thCheck-th2) > 0) {
      inRange = true;
    } else {
      inRange = false;  
    }
  } else { // th2 is more positive than th1
    if(angleRange(th2-thCheck) > 0 && angleRange(thCheck-th1) > 0) {
      inRange = true;
    } else {
      inRange = false;
    }
  }
  return inRange;
}