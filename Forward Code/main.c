/* Name: main.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"

#include "r_usb.h"
#include "r_led.h"
#include "r_rf.h"
#include "r_wii.h"
#include "r_motor_drive.h"
#include "r_parameters.h"
#include "r_hockey_playing.h"
#include "r_puck_sense.h"

char buffer[PACKET_LENGTH];
bool packet_received = false;
bool stop_flag = true;
unsigned char score_us = 0;
unsigned char score_them = 0;

/* Wii variables */
unsigned int blobs[] = {0,0,0,0,0,0,0,0,0,0,0,0}; // 12 element buffer for Wii data
unsigned int starLocPrev[] = {0,0,0,0,0,0,0,0}; // location of previous stars
bool missedPoint = false;
bool missedPointPrev = false; // whether we were missing a point previously
double theta; // robot's current angle wrt line from center to goal B
double positionX; // robot's current x position
double positionY; // robot's current y position

// NOTE - These variables must be retreived from switch values at init().
bool towardB = false; // direction of robot: 1 towards B, 0 towards A
bool isBlue = false; // Indicates team color of robot.

/* Method Declarations */
void init(void);
void assignDirection();

int main_motor_test();
int main_goal_test();
int main_get_location();
int main_puck_sense_test();

int main(void) {
  // main_motor_test();
  // main_goal_test();  
  // main_get_location();
  main_puck_sense_test();
  while(1);
}

// Reads new packet and sets variables accordingly.
int main_comm_test() {
	init();
	while(1) {
    processPacket();
	}
}

int main_find_puck() {
  init();
  while(1) {
    processPacket();
    assignDirection();
    getLocation();
    if(stop_flag) {
      driveLeftMotor(true, 1);
      driveRightMotor(true,1);
    } else {
      puckFind();
    }
  }
}

int main_puck_sense_test() {
  init();
  while(1) {
    assignDirection();
    if(check(PIND,LEFT_SENSOR)) {
      m_red(ON);
    } else {
      m_red(OFF);
    }
  }
}

// drives left motor forward then backward, and right motor backward then forward
int main_motor_test() {
  init();
  while(1) {
    driveLeftMotor(true,MOTOR_SPEED);
    driveRightMotor(false,MOTOR_SPEED);
    m_wait(2000);
    driveLeftMotor(false,MOTOR_SPEED);
    driveRightMotor(true,MOTOR_SPEED);
    m_wait(2000);
  }
}

// Causes robot to go to goal on opposite side of arena from starting position
int main_goal_test() {
  init();
  while(1) {
    assignDirection();
   if(!missedPoint) {
      m_red(ON);
    } else {
      m_red(OFF);
    }
    getLocation();
    goToGoal();
  }
}

// uses USB to print location
int main_get_location() {
  init();
  init_usb();
  assignDirection();
  while(1) {
    if(!missedPoint) {
      m_red(ON);
    } else {
      m_red(OFF);
    }
    getLocation();
    printLocation();
    m_wait(2000);
  }
}

// Assigns robot direction to go toward opposite side from starting position
void assignDirection() {
  if(check(PINF,LED_IN)) {
    towardB = true;
    blueOn();
  } else {
    towardB = false;
    redOn();
  }

  // getLocation();
  // assign direction
  // if(positionX < 0) {
  //   towardB = true;
  // } else {
  //   towardB = false;
  // }
}

// Overflow actions (enable both motors)
ISR(TIMER1_OVF_vect) {
  set(PORTB,LEFT_ENABLE);
  set(PORTB,RIGHT_ENABLE);
}

// Channel B actions (turn off left motor)
ISR(TIMER1_COMPB_vect) {
  clear(PORTB,LEFT_ENABLE);
}

// Channel C actions (turn off right motor)
ISR(TIMER1_COMPC_vect) {
  clear(PORTB,RIGHT_ENABLE);
}

// Activated when RF packet received
ISR(INT2_vect) {
  packet_received = true;
  m_rf_read(buffer, PACKET_LENGTH);
}