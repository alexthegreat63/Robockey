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