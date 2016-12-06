// -----------------------------------------------------------------------------
// M2 Wiimote Pixart Sensor interface
// version: 1.1
// date: Nov 11, 2011
// author: J. Fiene
// -----------------------------------------------------------------------------

#ifndef r_geometry__
#define r_geometry__

#include "m_general.h"
#include "r_parameters.h"

// -----------------------------------------------------------------------------
// Public functions:s
// -----------------------------------------------------------------------------

double angleRange(double th);
bool wrapCompare(double thCheck, double th1, double th2); // finds if thCheck is in smallest range of th1 and th2

#endif