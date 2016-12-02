// -----------------------------------------------------------------------------
// M2 Wiimote Pixart Sensor interface
// version: 1.1
// date: Nov 11, 2011
// author: J. Fiene
// -----------------------------------------------------------------------------

#ifndef r_motor_drive__
#define r_motor_drive__

#include "m_general.h"

// -----------------------------------------------------------------------------
// Public functions:s
// -----------------------------------------------------------------------------

void driveLeftMotor(bool direction, unsigned int duty);
void driveRightMotor(bool direction, unsigned int duty);

#endif