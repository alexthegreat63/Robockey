/* Name: main.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_general.h"
#include "m_bus.h"
#include "m_usb.h"
#include "m_wii.h"
#include "r_parameters.h"

extern bool packet_received;
extern bool stop_flag;
extern unsigned char score_us;
extern unsigned char score_them;

/* Wii variables */
extern unsigned int blobs[]; // 12 element buffer for Wii data
extern unsigned int starLocPrev[]; // location of previous stars
extern bool missedPoint;
extern bool missedPointPrev; // whether we were missing a point previously
extern double theta; // robot's current angle wrt line from center to goal B
extern double positionX; // robot's current x position
extern double positionY; // robot's current y position

// NOTE - These variables must be retreived from switch values at init().
extern bool towardB; // direction of robot: 1 towards B, 0 towards A
extern bool isBlue; // Indicates team color of robot.

// Initializes USB connection
void init_usb() {
  m_usb_init();
  while(!m_usb_isconnected()); // wait for a connection
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
}