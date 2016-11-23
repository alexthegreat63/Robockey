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
#include "m_wii.h"
// output pins to motor driver
#define SYSTEM_FREQ 16000000 // system clock frequency, Hz
#define LEFT_ENABLE 2
#define LEFT_DIRECTION 3
#define RIGHT_ENABLE 4
#define RIGHT_DIRECTION 5
#define CHANNEL 1
#define ADDRESS 78
#define PACKET_LENGTH 3 // not sure if 3 or 24

/*
 * Wii variables
 */
unsigned int blobs[] = {0,0,0,0,0,0,0,0,0,0,0,0}; // 12 element buffer for Wii data
unsigned int starLocPrev[] = {0,0,0,0,0,0,0,0};
bool missedPointPrev = false;
double theta;
double positionX;
double positionY;

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

  /*
   * Initialize Wii Module
   */

  m_wii_open();
}

/*
 * Drive the left motor forward
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
 * Drive the right motor forward
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

void getLocation() {
  m_wii_read(blobs); // update blobs
  //unsigned int starLocations[] = {0,0,0,0,0,0,0,0};
  int starLocSize = 8;
  bool missedPoint = false;

  missedPointPrev = missedPoint;
  missedPoint = false;
  unsigned int starLocations[] = {blobs[0], blobs[1], blobs[3], blobs[4], blobs[6], blobs[7], blobs[9], blobs[10]};
  int n;
  for(n=0;n<=3;n++) {
    if(starLocations[n] == 1023 || starLocations[n+4] == 1023) {
      if(missedPoint) {
        missedPoint = missedPointPrev;
        int z = 0;
        for(z =0; z<starLocSize; z++) {
          starLocations[z] = starLocPrev[z];
        }
        break;
      } else if(!missedPoint) {
        missedPoint = true;
      } if(n<3 && (starLocations[3] == 1023 || starLocations[7] == 1023)) {
        starLocations[n] = starLocations[3];
        starLocations[n+4] = starLocations[7];
      }
    }
  }

  int Pmax1 = 0;
  int Pmax2 = 1;
  int Dmax = 0;
  int P2max1 = 0;
  int P2max2 = 2;
  int D2max = 0;

  if(!missedPoint) {
    int i;
    for(i=0;i<=2;i++) {
      int j;
      for(j=i+1;j<=3;j++) {
        int d = pow(starLocations[i]-starLocations[j],2) + pow(starLocations[i+4]-starLocations[j+4],2);
        if(d>Dmax) {
          P2max1 = Pmax1;
          P2max2 = Pmax2;
          D2max = Dmax;
          Pmax1 = i;
          Pmax2 = j;
          Dmax = d;
        } else if(d>D2max) {
          P2max1 = i;
          P2max2 = j;
          D2max = d;  
        }
      }
    }
    int c; int a;
    if(Pmax1==P2max1 || Pmax1== P2max2) {
      c=Pmax1;
      a=Pmax2;
    } else {
      c=Pmax2;
      a=Pmax1;
    }
    theta = atan2(starLocations[a+4]-starLocations[c+4],starLocations[a]-starLocations[c]); // -pi/2
    positionX = (starLocations[a]+starLocations[c])/2-512;
    positionY = (starLocations[a+4]+starLocations[c+4])/2-384;
  }
}

int main(void) {
  init();
  driveLeftMotor(true,0.2);
  driveRightMotor(false,0.7);
  while(1) {
  }
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