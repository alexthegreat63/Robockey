/* Name: r_init.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_general.h"
#include "r_parameters.h"
#include "m_wii.h"
#include "m_bus.h"
#include "m_rf.h"

extern char buffer[10];
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

   clear(DDRD,FRONT_LEFT_SENSOR); // initialize for input
   clear(PORTD,FRONT_LEFT_SENSOR); // disable pullup resistor

   clear(DDRD,FRONT_RIGHT_SENSOR); // initialize for input
   clear(PORTD,FRONT_RIGHT_SENSOR); // disable pullup resistor

   clear(DDRD,PUCK_LEFT_SENSOR); // initialize for input
   clear(PORTD,PUCK_LEFT_SENSOR); // disable pullup resistor

   clear(DDRF,PUCK_RIGHT_SENSOR); // initialize for input
   clear(PORTF,PUCK_RIGHT_SENSOR); // disable pullup resistor

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

   m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH);

   //m_red(OFF);

  m_red(OFF);
}