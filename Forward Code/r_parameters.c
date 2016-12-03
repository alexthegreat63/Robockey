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
#define FRONT_SENSOR 5
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