/* Name: main.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 * 
 * Tournament instructions:
 * Forward:
 * Make sure stop_flag = true; in the main function
 * Comment out all mains but main competition function
 * Assign proper address (40 or 41) in r_parameters.h
 * Comment goalie interrupts at bottom; uncomment normal interrupts
 * 
 * Goalie:
 * Make sure stop_flag = true; in the main function
 * Comment out all forward interrupts at bottom; uncomment goalie interrupts
 * Comment all other main functions except main_goalie
 * make sure g_stopped is true
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

#include "g_goalkeeping.h"
#include "g_init.h"
#include "g_motor_drive.h"
#include "g_parameters.h"

char buffer[PACKET_LENGTH] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
bool packet_received = false;
bool stop_flag = true; // WHEN STOP COMMAND IS ISSUED, TRUE
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
// bool isBlue = false; // Indicates team color of robot.

bool left_stopped = true; // when left and right motors are stopped by us (not command)
bool right_stopped = true; // when left and right motors are stopped by us (not command)
bool g_stopped = true; // when goalie is stopped by us (not command)

/* Method Declarations */
void init(void);
void assignDirection();

int main_motor_test();
int main_goal_test();
int main_get_location();
int main_puck_sense_test();
int main_ir_test();
int main_find_puck();
int main_find_puck_no_comms();
int main_find_puck_test();
int main_comm_test();

int main_goalie(); // TODO: Write
int main_goalie_test();
int main_goalie_motor();

int main(void) {
  stop_flag = true;
  main_find_puck(); // THIS IS THE MAIN COMPETITION FUNCTION - contains comms
  // main_goal_test(); // test going to goal (no comms)
  // main_find_puck_no_comms(); // Test whole workflow with no comms
  // main_motor_test();
  // init_usb();
  // main_get_location();
  // screen /dev/tty.usbmodem411
  // main_puck_sense_test();
  // NOTE: ADD GOTOGOAL BACK TO PUCKFIND WHEN DONE TESTING
  // main_find_puck_test();
  // main_ir_test();
  // main_comm_test();

  /* Goalkeeper */
  // stop_flag = true;
  // main_goalie(); // COMPETITION FUNCTION FOR GOALIE
  // main_goalie_test();
  // main_goalie_motor();

  while(1);
}

int main_goalie() {
  g_init();
  while(1) {
    processPacket();
    assignDirection();
    getLocation();
    g_puckFind();
  }
}

// Performs finding puck and going to goal routine
int main_find_puck() {
  init();
  driveForward();
  while(1) { // if play command issued, will start
    assignDirection();
    processPacket();
    getLocation();
    if(stop_flag) {
      driveForward();
    } else {
      puckFind();
    }
  }
}

int main_goalie_test() {
  g_init();
  while(1) {
    processPacket();
    assignDirection();
    getLocation();
    g_puckFind();
  }
}

int main_goalie_motor() {
  g_init();
  while(1) {
    assignDirection();
    if(towardB) {
      g_driveMotor(true); // turn motors right
    } else {
      g_stop();
    }
  }
}

// Performs finding puck and going to goal routine
int main_find_puck_no_comms() {
  init();
  while(1) {
    assignDirection();
    processPacket();
    stop_flag = false;
    if(stop_flag) {
      stopLeft();
      stopRight();
    } else {
      getLocation();
      puckFind();
    }
  }
}

// Reads new packet and sets variables accordingly.
int main_comm_test() {
	init();
	while(1) {
    assignDirection();
    processPacket();
	}
}

int main_ir_test() {
  init();
  while(1) {
    if(!check(PIND,4)) {
      m_red(ON);
    } else {
      m_red(OFF);
    }
  }
}

// Performs finding puck and going to goal routine
int main_find_puck_test() {
  init();
  while(1) {
    assignDirection();
    // processPacket();
    // getLocation();
    puckFind();
  }
}

// Checks to see if left sensors are working
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
    blueOn();
    // driveLeftMotor(true,MOTOR_SPEED_FORWARD);
    // driveRightMotor(true,MOTOR_SPEED_FORWARD);
    //driveRightMotor(false,MOTOR_SPEED);
    //m_wait(2000);
    //driveLeftMotor(false,MOTOR_SPEED);
    //driveRightMotor(true,MOTOR_SPEED);
    //m_wait(2000);
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
    redOn();
  } else {
    towardB = false;
    blueOn();
  }
}

/** Forward motor interrupts */

ISR(TIMER1_OVF_vect) { /** Overflow actions (enable both motors) */
  if(!left_stopped && !stop_flag) {
   set(PORTB,LEFT_ENABLE);
  }
  if(!right_stopped && !stop_flag) {
    set(PORTB,RIGHT_ENABLE);
  }
}


ISR(TIMER1_COMPB_vect) { /** Channel B actions (turn off left motor)  */
  clear(PORTB,LEFT_ENABLE);
}
 
ISR(TIMER1_COMPC_vect) { /** Channel C actions (turn off right motor) */
  clear(PORTB,RIGHT_ENABLE);
}

/** Goalie motor interrupts */

// ISR(TIMER1_OVF_vect) { /* Overflow actions (enable both motors) */
//   if(!g_stopped && !stop_flag) {
//    set(PORTB,G_ENABLE_1);
//    set(PORTB,G_ENABLE_2);
//   }
// }


// ISR(TIMER1_COMPB_vect) { /** Channel B actions (turn off left motor)  */
//   clear(PORTB,G_ENABLE_1);
//   clear(PORTB,G_ENABLE_2);
// }

/** Activated when RF packet received */
ISR(INT2_vect) {
  packet_received = true;
}