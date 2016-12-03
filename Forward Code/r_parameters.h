// -----------------------------------------------------------------------------
// M2 Wiimote Pixart Sensor interface
// version: 1.1
// date: Nov 11, 2011
// author: J. Fiene
// -----------------------------------------------------------------------------

#ifndef r_parameters__
#define r_parameters__

// output pins to motor driver. All Port B.
#define SYSTEM_FREQ 16000000 // system clock frequency, Hz
#define RIGHT_ENABLE 0
#define RIGHT_DIRECTION 1
#define LEFT_ENABLE 2
#define LEFT_DIRECTION 3

// input pins for IR sensors. All Port D.
#define LEFT_SENSOR 3
#define RIGHT_SENSOR 4
#define FRONT_LEFT_SENSOR 5
#define FRONT_RIGHT_SENSOR 7
#define FRONT_CENTER_SENSOR 1
#define PUCK_SENSOR 6

// output pins for blue/red LEDs. All Port F.
#define BLUE 5
#define RED 6
#define LED_IN 7

// Motor Parameters:
#define MOTOR_SPEED 0xFFFF/4

//RF Parameters:
#define CHANNEL 1
#define ADDRESS 40 // address for robot 1
#define PACKET_LENGTH 10 // bytes

 // Important locations
#define X_GOAL_B 247 // -310
#define Y_GOAL_B_HIGH 187 // 83 // 150
#define Y_GOAL_B_LOW -15 //-42 // 20
#define X_GOAL_A -247
#define Y_GOAL_A_HIGH 187
#define Y_GOAL_A_LOW -15

// Distance behind other robot for train
#define TRAIN_DISTANCE 15
#define TRAIN_THETA_PAD 0.1 // + or - in x and y to get to location

// -----------------------------------------------------------------------------
// Public functions:s
// -----------------------------------------------------------------------------

#endif