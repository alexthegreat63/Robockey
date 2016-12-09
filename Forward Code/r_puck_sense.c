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
#include "r_led.h"

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

extern volatile unsigned int puck_count; // number of total cycles to get 250ms delay. /0.25s((65535 clock cycles/timer overflow)/(16MHz))
extern volatile bool timer_on; // turns on when we want the delay clock going, otherwise off

// Seek puck
void puckFind() {
  if(!check(PIND, PUCK_LEFT_SENSOR) && !check(PIND, PUCK_RIGHT_SENSOR)) { // if puck is in our posession
    // m_wait(250);
    /* wait 250 ms */
    puck_count = 0;
    timer_on = true;
    while(puck_count < 61) {
    }
    timer_on = false;

    m_red(ON);
    // driveForward();
    m_green(ON);
    goToGoal();
  } else if(!check(PIND, FRONT_LEFT_SENSOR) && !check(PIND, FRONT_RIGHT_SENSOR)) { // if puck is directly ahead
    // m_wait(1000);
    driveLeftMotor(true, MOTOR_SPEED_FORWARD);
    driveRightMotor(true, MOTOR_SPEED_FORWARD);
    m_red(OFF);
    m_green(ON);
  } 
  // else if(!check(PIND,FRONT_LEFT_SENSOR)) {
  //   stopLeft();
  //   driveRightMotor(true,MOTOR_SPEED_ROTATE);
  // } else if(!check(PIND,FRONT_RIGHT_SENSOR)) {
  //   driveLeftMotor(true,MOTOR_SPEED_ROTATE);
  //   stopRight();
  // } 
  else if(!check(PIND,LEFT_SENSOR)) { // if puck is to left
    stopLeft();
    // driveRightMotor(true, MOTOR_SPEED_ROTATE);
    driveRightMotor(true, MOTOR_SPEED_FORWARD);
    m_red(ON);
    m_green(OFF);
  } else if(!check(PIND,RIGHT_SENSOR)) { // if puck is to right
    // driveLeftMotor(true, MOTOR_SPEED_ROTATE);
    driveLeftMotor(true, MOTOR_SPEED_FORWARD);
    stopRight();
    m_green(OFF);
    m_red(ON);
  } 
  // NOTE: May have to change the order of the if statements. Also, may call goToGoal() from here if PUCK_SENSOR is high.
}