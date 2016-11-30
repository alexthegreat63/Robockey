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
// output pins to motor driver. All Port B.
#define SYSTEM_FREQ 16000000 // system clock frequency, Hz
#define RIGHT_ENABLE 0
#define RIGHT_DIRECTION 1
#define LEFT_ENABLE 2
#define LEFT_DIRECTION 3
// input pins for IR sensors. All Port D.
#define LEFT_SENSOR 3
#define RIGHT_SENSOR 4
#define FRONT_SENSOR 5
#define PUCK_SENSOR 6
// output pins for blue/red LEDs. All Port F.
#define BLUE 6
#define RED 7
//RF Parameters:
#define CHANNEL 1
#define ADDRESS 40 // address for robot 1
#define PACKET_LENGTH 10 // bytes
char buffer[PACKET_LENGTH];
bool packet_received = false;
bool stop_flag = true;
unsigned char score_us = 0;
unsigned char score_them = 0;
// Important locations
#define X_GOAL_B 247 // -310
#define Y_GOAL_B_HIGH 187 // 83 // 150
#define Y_GOAL_B_LOW -15 //-42 // 20

#define X_GOAL_A -247
#define Y_GOAL_A_HIGH 187
#define Y_GOAL_A_LOW -15

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
bool towardB; // direction of robot: 1 towards B, 0 towards A

/* Method Declarations */
void init(void);
void init_usb(void);
void driveLeftMotor(bool direction, unsigned int duty);
void driveRightMotor(bool direction, unsigned int duty);
void getLocation();
void assignDirection();
void printLocation();
void goToGoal();

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
		if(packet_received == true) {
			packet_received = false;
			m_rf_read(buffer, PACKET_LENGTH);
			
			if(buffer(0) == 0xA0) { // Comm Test
				stop_flag = true;
			}
			else if(buffer(0) == 0xA1) { // Play
				stop_flag = false;
			}
			else if(buffer(0) == 0xA2) { // Goal R
				stop_flag = true;
				if(towardB == 0) {
					score_us = buffer(2);
					score_them = buffer(3);
				}
				else {
					score_us = buffer(3);
					score_them = buffer(2);
				}
			}
			else if(buffer(0) == 0xA3) { // Goal B
				stop_flag = true;
				if(towardB == 0) {
					score_us = buffer(2);
					score_them = buffer(3);
				}
				else {
					score_us = buffer(3);
					score_them = buffer(2);
				}
			}
			else if(buffer(0) == 0xA4) { // Pause
				stop_flag = true;
			}
			else if(buffer(0) == 0xA6) { // Halftime
				stop_flag = true;
				towardB = !towardB;
			}
			else if(buffer(0) == 0xA7) { // Game Over
				stop_flag = true; 
			}
			else {
				// Nothing
			}
		}
	}
}

int main_puck_sense_test() {
  init();
  while(1) {
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
  assignDirection();
  while(1) {
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
   * Input Pins (Puck IR sensors)
   */

   clear(DDRD,LEFT_SENSOR); // initialize for input
   clear(PORTD,LEFT_SENSOR); // disable pullup resistor

   clear(DDRD,RIGHT_SENSOR); // initialize for input
   clear(PORTD,RIGHT_SENSOR); // disable pullup resistor

   clear(DDRD,FRONT_SENSOR); // initialize for input
   clear(PORTD,FRONT_SENSOR); // disable pullup resistor

   clear(DDRD,PUCK_SENSOR); // initialize for input
   clear(PORTD,PUCK_SENSOR); // disable pullup resistor

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

// Initializes USB connection
void init_usb() {
  m_usb_init();
  while(!m_usb_isconnected()); // wait for a connection
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

// updates robot location
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

// Assigns robot direction to go toward opposite side from starting position
void assignDirection() {
  getLocation();
  // assign direction
  if(positionX < 0) {
    towardB = true;
  } else {
    towardB = false;
  }
}

// Prints angle*100, x position, and y position in arena
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

// Causes robot to go directly to desired goal
void goToGoal() {
  int y_high;
  int y_low;
  int x;
  if(towardB) {
    y_high = Y_GOAL_B_HIGH;
    y_low = Y_GOAL_B_LOW;
    x = X_GOAL_B;
  } else {
    y_high = Y_GOAL_A_HIGH;
    y_low = Y_GOAL_A_LOW;
    x = X_GOAL_A;
  }
  double theta_goal_high = -atan2(y_high-positionY,x-positionX); // angle to high side of the goal
  double theta_goal_low = -atan2(y_low-positionY,x-positionX); // angle to low side of the goal
  if(theta > 3.14159) {
    theta -= 2*3.14159;
  } else if (theta < -3.14159) {
    theta+= 2*3.14159;
  }
  if(theta-theta_goal_high<0) { // rotate right
    m_green(OFF);
    driveRightMotor(true,0xFFFF/4);
    driveLeftMotor(false,0xFFFF/4);
  } else if(theta-theta_goal_low>0) { // rotate left
    m_green(OFF);
    driveRightMotor(false,0xFFFF/4);
    driveLeftMotor(true,0xFFFF/4);
  } else {
    m_green(ON);
    driveLeftMotor(true,0xFFFF/4);
    driveRightMotor(true,0xFFFF/4);
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
ISR(INT2_vect) {
	packet_received = true;
}