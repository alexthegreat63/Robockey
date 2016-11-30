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
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
// output pins to motor driver
#define SYSTEM_FREQ 16000000 // system clock frequency, Hz
#define RIGHT_ENABLE 0
#define RIGHT_DIRECTION 1
#define LEFT_ENABLE 2
#define LEFT_DIRECTION 3
//RF Parameters
#define CHANNEL 1
#define ADDRESS 40 // address for robot 1
#define PACKET_LENGTH 10 // bytes
char buffer[PACKET_LENGTH];
bool packet_received = false;
// Important locations: currently arbitrary
#define X_GOAL_B 247 // -310
#define Y_GOAL_B_HIGH 187 // 83 // 150
#define Y_GOAL_B_LOW -15 //-42 // 20
/*
 * Wii variables
 */
unsigned int blobs[] = {0,0,0,0,0,0,0,0,0,0,0,0}; // 12 element buffer for Wii data
unsigned int starLocPrev[] = {0,0,0,0,0,0,0,0}; // location of previous stars
bool missedPoint = false;
bool missedPointPrev = false; // whether we were missing a point previously
double theta; // robot's current angle wrt line from center to goal B
double positionX; // robot's current x position
double positionY; // robot's current y position

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
  // set outputs to 0 to start:
  clear(PORTB,LEFT_ENABLE);
  clear(PORTB,LEFT_DIRECTION);
  clear(PORTB,RIGHT_ENABLE);
  clear(PORTB,RIGHT_DIRECTION);

  /*
   * Initialize Wii Module
   */

  m_wii_open();

  /*
   * Initialize mRF module
   */

   //m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH);

   //m_red(OFF);

   /*
    * USB Initialization
    */

  // m_usb_init();
  // while(!m_usb_isconnected()); // wait for a connection

  m_red(OFF);
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
  missedPointPrev = missedPoint;
  missedPoint = false;

  int starLocations[] = {blobs[0], blobs[3], blobs[6], blobs[9], blobs[1], blobs[4], blobs[7], blobs[10]};
  int n;
  for(n=0;n<=3;n++) {
    if(starLocations[n] == 1023 || starLocations[n+4] == 1023) {
     // if(missedPoint) {
        missedPoint = true; // TODO: fix later
        int z = 0;
        for(z =0; z<starLocSize; z++) {
          starLocations[z] = starLocPrev[z];
        }
        break;
      //} //else if(!missedPoint) {
        //missedPoint = true;
      //} 
      // if(n<3 && (starLocations[3] == 1023 || starLocations[7] == 1023)) {
      //   starLocations[n] = starLocations[3];
      //   starLocations[n+4] = starLocations[7];
      // }
    }
  }

  int Pmax1 = 0;
  int Pmax2 = 1;
  int Dmax = 0;
  int P2max1 = 0;
  int P2max2 = 2;
  int D2max = 0;

  /*
   * Case: no missing point
   */
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
    // Transform into final position and orientation
    double theta_frame = -3.14159/2+atan2(((double)starLocations[a+4]-(double)starLocations[c+4]), ((double)starLocations[a]-(double)starLocations[c])); // -pi/2
    //3.14159/2+
    double positionX_frame = (starLocations[a]+starLocations[c])/2.-512.;
    double positionY_frame = (starLocations[a+4]+starLocations[c+4])/2.-384.;

  //   m_usb_tx_char('t');
  //   m_usb_tx_char('a');
  // m_usb_tx_int((int) (theta_frame*100));
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('x');
  // m_usb_tx_int(positionX_frame);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('y');
  // m_usb_tx_int(positionY_frame);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');

    positionX = -positionX_frame*cos(theta_frame)-positionY_frame*sin(theta_frame);
    positionY = positionX_frame*sin(theta_frame)-positionY_frame*cos(theta_frame);
    theta = theta_frame;
    //positionX = positionX_frame;
    //positionY = positionY_frame;
    //theta = theta_frame;
  }
}

void printLocation() {
  m_usb_tx_char('a');
  m_usb_tx_int((int) (theta*100));
  m_usb_tx_char('\r');
  m_usb_tx_char('\n');
  m_usb_tx_char('x');
  m_usb_tx_int(positionX);
  m_usb_tx_char('\r');
  m_usb_tx_char('\n');
  m_usb_tx_char('y');
  m_usb_tx_int(positionY);
  m_usb_tx_char('\r');
  m_usb_tx_char('\n');
  m_usb_tx_char('\r');
  m_usb_tx_char('\n');

  //   m_usb_tx_char('a');
  // m_usb_tx_int(blobs[0]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('x');
  // m_usb_tx_int(blobs[1]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('y');
  // m_usb_tx_int(blobs[3]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  //   m_usb_tx_int(blobs[4]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  //   m_usb_tx_int(blobs[6]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  //   m_usb_tx_int(blobs[7]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  //   m_usb_tx_int(blobs[9]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  //   m_usb_tx_int(blobs[10]);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');

}

void goToGoal() {
  double theta_goal_high = atan2(Y_GOAL_B_HIGH-positionY,X_GOAL_B-positionX); // angle to high side of the goal
  double theta_goal_low = atan2(Y_GOAL_B_LOW-positionY,X_GOAL_B-positionX); // angle to low side of the goal
  if(theta > 3.14159) {
    theta -= 2*3.14159;
  } else if (theta < -3.14159) {
    theta+= 2*3.14159;
  }
  if(theta-theta_goal_high>0) { // rotate right
    m_green(OFF);
    driveLeftMotor(true,0xFFFF/4);
    driveRightMotor(false,0xFFFF/4);
  } else if(theta-theta_goal_low<0) { // rotate left
    m_green(OFF);
    driveLeftMotor(false,0xFFFF/4);
    driveRightMotor(true,0xFFFF/4);
  } else {
    m_green(ON);
    driveLeftMotor(true,0xFFFF/4);
    driveRightMotor(true,0xFFFF/4);
  }
}

void motorTest(void) {
  driveLeftMotor(true,0xFFFF/2);
  driveRightMotor(false,0xFFFF/2);
  m_wait(2000);
  driveLeftMotor(false,0xFFFF/2);
  driveRightMotor(true,0xFFFF/2);
  m_wait(2000);
}

int main(void) {
  init();
  //motorTest();
  while(1) {
    if(!missedPoint) {
      m_red(ON);
    } else {
      m_red(OFF);
    }
    /** Go to goal code */
    getLocation();
    goToGoal();

    /** USB location code */
    // getLocation();
    // printLocation();
    // m_wait(2000);

    // driveLeftMotor(true,0.5*0xFFFF);
    // driveRightMotor(true,0.5*0xFFFF);
    // m_wait(2000);
    //motorTest();
    //if(packet_received) {
      //goToGoal();
    //}
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

// Activated when RF packet received
// ISR(INT2_vect) {
//   packet_received = true;
//   m_rf_read(buffer, PACKET_LENGTH);
// }