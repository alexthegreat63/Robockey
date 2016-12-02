/* Name: r_rf.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"

#include "r_led.h"
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

void processPacket() {
  if(packet_received == true) {
    packet_received = false;
    m_rf_read(buffer, PACKET_LENGTH);
    
    if(buffer[0] == 0xA0) { // Comm Test
      stop_flag = true;
    if(isBlue) {
      blueOn();
      m_wait(1000);
      ledOff();
    }
    else {
      redOn();
      m_wait(1000);
      ledOff();
    }
    }
    else if(buffer[0] == 0xA1) { // Play
      stop_flag = false;
    if(isBlue) {
      blueOn();
    }
    else {
      redOn();
    }
    }
    else if(buffer[0] == 0xA2) { // Goal R
      stop_flag = true;
      if(towardB == 0) {
        score_us = buffer[2];
        score_them = buffer[3];
      }
      else {
        score_us = buffer[3];
        score_them = buffer[2];
      }
    }
    else if(buffer[0] == 0xA3) { // Goal B
      stop_flag = true;
      if(towardB == 0) {
        score_us = buffer[2];
        score_them = buffer[3];
      }
      else {
        score_us = buffer[3];
        score_them = buffer[2];
      }
    }
    else if(buffer[0] == 0xA4) { // Pause
      stop_flag = true;
    }
    else if(buffer[0] == 0xA6) { // Halftime
      stop_flag = true;
    }
    else if(buffer[0] == 0xA7) { // Game Over
      stop_flag = true; 
    ledOff();
    }
    else {
      // Nothing
    }
  }
}