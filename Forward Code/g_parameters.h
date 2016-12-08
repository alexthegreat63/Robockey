// -----------------------------------------------------------------------------
// M2 Wiimote Pixart Sensor interface
// version: 1.1
// date: Nov 11, 2011
// author: J. Fiene
// -----------------------------------------------------------------------------

#ifndef g_parameters__
#define g_parameters__

// output pins to motor driver. All Port B.
#define G_ENABLE_1 0
#define G_LEFT_DIRECTION_1 1
#define G_ENABLE_2 2
#define G_LEFT_DIRECTION_2 3

// input pins for IR sensors. All Port D.
#define G_LEFT_SENSOR 5
#define G_RIGHT_SENSOR 3
#define G_FRONT_SENSOR 4
// #define PUCK_SENSOR 6

#define G_X_GOAL_RED -263 
#define G_Y_GOAL_RED_HIGH 35
#define G_Y_GOAL_RED_LOW -118
#define G_X_GOAL_BLUE 310
#define G_Y_GOAL_BLUE_HIGH 84
#define G_Y_GOAL_BLUE_LOW -49

// output pins for blue/red LEDs. All Port F.
// #define G_BLUE 5
// #define G_RED 6
// #define G_COLOR_IN 7

#endif