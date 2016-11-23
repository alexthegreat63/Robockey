/* Name: main.c
 * Author: Alex Kearns
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
//#include "m_rf.h"
#include "m_usb.h"
// output pins to motor driver
#define SYSTEM_FREQ 16000000 // system clock frequency, Hz
#define LEFT_ENABLE 2
#define LEFT_DIRECTION 3
#define RIGHT_ENABLE 4
#define RIGHT_DIRECTION 5
#define CHANNEL 1
#define ADDRESS 78
#define PACKET_LENGTH 3 // not sure if 3 or 24

void init(void) {
  m_clockdivide(0); // set system clock speed
  m_red(ON);

  /*
   * m_bus initialization: DO NOT use pins D0, D1, D2
   */

  m_bus_init();

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
  OCR1B = 0;
  set(DDRB,6); // enable output for channel B (left motor)
  set(TIMSK1,OCIE1B); // enable ch B interrupt

  // Channel B compare output options (set flag)
  set(TCCR1A,COM1B1);
  set(TCCR1A,COM1B0);

  /* Channel C setup */
  OCR1C = 0;
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
  // set outputs to 0 to start:
  clear(PORTB,LEFT_ENABLE);
  clear(PORTB,LEFT_DIRECTION);
  clear(PORTB,RIGHT_ENABLE);
  clear(PORTB,RIGHT_DIRECTION);
}

/*
 * Drive the left motor
 * Inputs: 
 *  direction (1 forward, 0 backward)
 *  duty (0xFFFF = 100% duty, 0 = 0% duty)
 */
void driveLeftMotor(bool direction, unsigned int duty) {
  if(direction) {
    set(PORTB,LEFT_DIRECTION);
  } else {
    clear(PORTB,LEFT_DIRECTION);
  }
  OCR1B = duty;
}

/*
 * Drive the right motor
 * Inputs: 
 *  direction (1 forward, 0 backward)
 *  duty (0xFFFF = 100% duty, 0 = 0% duty)
 */
void driveRightMotor(bool direction, unsigned int duty) {
  if(direction) {
    set(PORTB,RIGHT_DIRECTION);
  } else {
    clear(PORTB,RIGHT_DIRECTION);
  }
  OCR1C = duty;
}

// void seekPuck() {
//   if(one of the left sensors is active) {
//     driveRightMotor(1,0xFFFF/2);
//     driveLeftMotor(0,0xFFFF/2);
//   } else if(one of the right sensors is active) {
//     driveRightMotor(0,0xFFFF/2);
//     driveLeftMotor(1,0xFFFF/2);
//   } else if (center sensor active && !havePuck) {
//     driveRightMotor(1,0xFFFF/2);
//     driveLeftMotor(1,0xFFFF/2);
//   } else if(havePuckSensorActive) {
//     havePuck = true;
//     goToGoal();
//   } else if(one of our guys has the puck){
//     go to defense?
//   } else {
//     get into defense? search around for puck?
//   }
// }

int main(void) {
  init();
  driveLeftMotor(true,0.2);
  driveRightMotor(false,0.7);
  while(1);
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