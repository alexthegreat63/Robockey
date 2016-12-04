/* Name: r_rf.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

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
#define PACKET_LENGTH 10 // bytes

 // Important locations
#define X_GOAL_RED -263
#define Y_GOAL_RED_HIGH 35
#define Y_GOAL_RED_LOW -111
#define X_GOAL_BLUE 322
#define Y_GOAL_BLUE_HIGH 74
#define Y_GOAL_BLUE_LOW -73

// Distance behind other robot for train
#define TRAIN_DISTANCE 15
#define TRAIN_THETA_PAD 0.1 // + or - in x and y to get to location