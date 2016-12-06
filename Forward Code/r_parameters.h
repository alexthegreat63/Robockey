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
#define FRONT_LEFT_SENSOR 5 // D5
#define FRONT_RIGHT_SENSOR 7
#define PUCK_LEFT_SENSOR 6 // D6
#define PUCK_RIGHT_SENSOR 7 // PORT F

// output pins for blue/red LEDs. All Port F.
#define BLUE 1
#define RED 5
#define LED_IN 0

// Motor Parameters:
#define MOTOR_SPEED 0xFFFF/4

//RF Parameters:
#define CHANNEL 1
#define ADDRESS 40 // address for robot 1
#define GOALIE_ADDRESS 42 // address for goalie
#define PACKET_LENGTH 10 // bytes

// Important locations
 // Wu and Chen
// #define X_GOAL_RED -263 
// #define Y_GOAL_RED_HIGH 35
// #define Y_GOAL_RED_LOW -111
// #define X_GOAL_BLUE 322
// #define Y_GOAL_BLUE_HIGH 74
// #define Y_GOAL_BLUE_LOW -73
 //Levin Lobby
#define X_GOAL_RED -263 
#define Y_GOAL_RED_HIGH 35
#define Y_GOAL_RED_LOW -118
#define X_GOAL_BLUE 310
#define Y_GOAL_BLUE_HIGH 84
#define Y_GOAL_BLUE_LOW -49

// Distance behind other robot for train
#define TRAIN_DISTANCE 15
#define TRAIN_THETA_PAD 0.1 // + or - in x and y to get to location

#endif