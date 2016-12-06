// -----------------------------------------------------------------------------
// M2 Wiimote Pixart Sensor interface
// version: 1.1
// date: Nov 11, 2011
// author: J. Fiene
// -----------------------------------------------------------------------------

#ifndef g_motor_drive__
#define g_motor_drive__

#include "m_general.h"

// -----------------------------------------------------------------------------
// Public functions:s
// -----------------------------------------------------------------------------

void g_driveMotor(bool direction, unsigned int duty);
void g_stop();

#endif