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
    driveLeftMotor(true,0xFFFF/2);
    driveRightMotor(false,0xFFFF/2);
    m_wait(2000);
    driveLeftMotor(false,0xFFFF/2);
    driveRightMotor(true,0xFFFF/2);
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

// Initialization
void init(void) {
  m_clockdivide(0); // set system clock speed
  m_red(ON);

  /*
   * m_bus initialization: DO NOT use pins D0, D1, D2
   */

  m_bus_init();

  /**
   * Disable JTag to use pins F4-F7 as GPIO
   */

   m_disableJTAG();

  /*
   * Motor Timer Setups
   */
  // clock source: set to /1 of system clock
  clear(TCCR1B,CS12);
  clear(TCCR1B,CS11);
  set(TCCR1B,CS10);

  // timer mode 15: up to OCR1A, down to 0x000
  set(TCCR1B,WGM13);
  set(TCCR1B,WGM12);
  set(TCCR1A,WGM11);
  set(TCCR1A,WGM10);

  OCR1A = 0xFFFF;

  /* Channel A (overflow) setup */
  set(DDRB,5); // enable output for channel A (overflow)
  set(TIMSK1,TOIE1); // enable overflow interrupt

  /* Channel B setup */ 
  OCR1B = 1;
  set(DDRB,6); // enable output for channel B (left motor)
  set(TIMSK1,OCIE1B); // enable ch B interrupt

  // Channel B compare output options (set flag)
  set(TCCR1A,COM1B1);
  set(TCCR1A,COM1B0);

  /* Channel C setup */
  OCR1C = 1;
  set(DDRB,7); // enable output for channel C (right motor)
  set(TIMSK1,OCIE1C); // enable ch C interrupt

  // Channcel C compare output options
  set(TCCR1A,COM1C1);
  set(TCCR1A,COM1C0);

  sei(); // enable global interrupts

 /*
  * Output pins
  */
  // Enable pins as outputs:
  set(DDRB,LEFT_ENABLE);
  set(DDRB,LEFT_DIRECTION);
  set(DDRB,RIGHT_ENABLE);
  set(DDRB,RIGHT_DIRECTION);
  set(DDRF,BLUE);
  set(DDRF,RED);
  // set outputs to 0 to start:
  clear(PORTB,LEFT_ENABLE);
  clear(PORTB,LEFT_DIRECTION);
  clear(PORTB,RIGHT_ENABLE);
  clear(PORTB,RIGHT_DIRECTION);
  clear(PORTF,BLUE);
  clear(PORTF,RED);

  /*
   * Input Pins
   */

   // Puck IR sensors
   clear(DDRD,LEFT_SENSOR); // initialize for input
   clear(PORTD,LEFT_SENSOR); // disable pullup resistor

   clear(DDRD,RIGHT_SENSOR); // initialize for input
   clear(PORTD,RIGHT_SENSOR); // disable pullup resistor

   clear(DDRD,FRONT_SENSOR); // initialize for input
   clear(PORTD,FRONT_SENSOR); // disable pullup resistor

   clear(DDRD,PUCK_SENSOR); // initialize for input
   clear(PORTD,PUCK_SENSOR); // disable pullup resistor

   // LED Switch input
   clear(DDRF,LED_IN); // initialize for input
   set(PORTF,LED_IN); // enable pullup resistor

  /*
   * Initialize Wii Module
   */

  m_wii_open();

  /*
   * Initialize mRF module
   */

   //m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH);

   //m_red(OFF);

  m_red(OFF);
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