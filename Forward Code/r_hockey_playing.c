/* Name: r_hockey_playing.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include "m_usb.h"
#include "m_general.h"
#include "r_motor_drive.h"
#include "r_parameters.h"
#include "r_geometry.h"

extern char buffer[PACKET_LENGTH];
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

// Causes robot to go directly to desired goal
void goToGoal() {
  double y_high;
  double y_low;
  double x;
  if(towardB) {
    y_high = Y_GOAL_BLUE_HIGH;
    y_low = Y_GOAL_BLUE_LOW;
    x = X_GOAL_BLUE;
  } else {
    y_high = Y_GOAL_RED_HIGH;
    y_low = Y_GOAL_RED_LOW;
    x = X_GOAL_RED;
  }

  double theta_goal_high = atan2(y_high-positionY,x-positionX); // angle to high side of the goal
  double theta_goal_low = atan2(y_low-positionY,x-positionX); // angle to low side of the goal
  

  // if(theta-theta_goal_high<0) { // rotate right
  //   m_green(OFF);
  //   driveRightMotor(true,MOTOR_SPEED);
  //   driveLeftMotor(false,MOTOR_SPEED);
  // } else if(theta-theta_goal_low>0) { // rotate left
  //   m_green(OFF);
  //   driveRightMotor(false,MOTOR_SPEED);
  //   driveLeftMotor(true,MOTOR_SPEED);
  // } else {
  //   m_green(ON);
  //   driveLeftMotor(true,MOTOR_SPEED);
  //   driveRightMotor(true,MOTOR_SPEED);
  // }

  //a183
  //h-277
  //l-251

  // if(x<0) {
  //   double avg = (theta_goal_high+theta_goal_low)/2+3.14159;
  //   avg = angleRange(avg);

  //   if(theta<theta_goal_high && theta >= avg) {
  //     driveRightMotor(true,MOTOR_SPEED);
  //     driveLeftMotor(false,MOTOR_SPEED);
  //     m_green(OFF);
  //   } else if(theta>theta_goal_low && theta <= avg) {
  //     driveRightMotor(false,MOTOR_SPEED);
  //     driveLeftMotor(true,MOTOR_SPEED);
  //     m_green(OFF);
  //   } else {
  //     m_green(ON);
  //     driveRightMotor(true,MOTOR_SPEED);
  //     driveLeftMotor(true,MOTOR_SPEED);
  //   }
  // } else {
  //   double avg = (theta_goal_high+theta_goal_low)/2+3.14159;
  //   avg = angleRange(avg);
  //   if(theta>theta_goal_high && theta <= avg) {
  //     driveRightMotor(false,MOTOR_SPEED);
  //     driveLeftMotor(true,MOTOR_SPEED);
  //     m_green(OFF);
  //   } else if(theta<theta_goal_low && theta >= avg) {
  //     driveRightMotor(false,MOTOR_SPEED);
  //     driveLeftMotor(true,MOTOR_SPEED);
  //     m_green(OFF);
  //   } else {
  //     driveRightMotor(true,MOTOR_SPEED);
  //     driveLeftMotor(true,MOTOR_SPEED);
  //     m_green(ON);
  //   }
  // }
  
  double midpointCompare;
  midpointCompare = angleRange(angleRange((theta_goal_low+theta_goal_high)/2+M_PI));
  if(wrapCompare(midpointCompare,theta_goal_low,theta_goal_high)) {
    midpointCompare = angleRange(midpointCompare+M_PI);
  }


  // double tl;
  // double th;
  // if(!towardB) {
  //   if (theta_goal_low < 0) {
  //     tl = theta_goal_low + 2*M_PI;
  //   } else {
  //     tl = theta_goal_low;
  //   }
  //   if (theta_goal_high < 0) {
  //     th = theta_goal_high + 2*M_PI;
  //   } else {
  //     th = theta_goal_high;
  //   }
  //   midpointCompare = angleRange(angleRange((tl+th)/2+M_PI));
  // } else {
  //   midpointCompare = angleRange(angleRange((tl+th)/2+M_PI));
  // }


  // if (towardB) {
  //   midpointCompare = M_PI;
  // } else {
  //   midpointCompare = 0;
  // }
  // if(towardB) {
  //   midpointCompare = angleRange((theta_goal_low+theta_goal_high)/2+M_PI);
  // } else {
  //   midpointCompare = angleRange((theta_goal_low+theta_goal_high)/2);
  // }
  if(wrapCompare(theta,theta_goal_high,midpointCompare)) {
    m_green(OFF);
    if(towardB) {
      driveRightMotor(true,MOTOR_SPEED);
      driveLeftMotor(false,MOTOR_SPEED);
    } else {
      driveRightMotor(false,MOTOR_SPEED);
      driveLeftMotor(true,MOTOR_SPEED);
    }
  } else if(wrapCompare(theta,theta_goal_low,midpointCompare)) {
    m_green(OFF);
    if(towardB) {
      driveRightMotor(false,MOTOR_SPEED);
      driveLeftMotor(true,MOTOR_SPEED);
    } else {
      driveRightMotor(true,MOTOR_SPEED);
      driveLeftMotor(false,MOTOR_SPEED);
    }
  } else {
    m_green(ON);
    driveRightMotor(true,MOTOR_SPEED);
    driveLeftMotor(true,MOTOR_SPEED);
  }
  //   m_usb_tx_char('x');
  // m_usb_tx_int(positionX);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  //   m_usb_tx_char('y');
  // m_usb_tx_int(positionY);
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('a');
  // m_usb_tx_int((int) (theta*100));
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('h');
  // m_usb_tx_int((int) (theta_goal_high*100));
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('l');
  // m_usb_tx_int((int) (theta_goal_low*100));
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('m');
  // m_usb_tx_int((int) (midpointCompare*100));
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_usb_tx_char('\r');
  // m_usb_tx_char('\n');
  // m_wait(500);
}

void engageTrain(int otherX, int otherY, double otherTheta) {
  double otherUnitX = cos(otherTheta); // direction vector x component
  double otherUnitY = sin(otherTheta); // direction vector y component
  int aimForX = otherX - otherUnitX * TRAIN_DISTANCE; // x position to aim for
  int aimForY = otherY - otherUnitY * TRAIN_DISTANCE; // y position to aim for

  // Compute angle our robot should take, with padding on both sides
  double thetaToOther = atan2(aimForY-positionY,aimForX-positionX);
  double thetaAimHigh = thetaToOther + TRAIN_THETA_PAD;
  double thetaAimLow = thetaToOther - TRAIN_THETA_PAD;
  double midpointCompare = angleRange((thetaAimHigh+thetaAimLow)/2+M_PI);
  // if we're out of range of the target point
  if(pow(aimForX-positionX,2)+pow(aimForY-positionY,2) > TRAIN_DISTANCE) {
    if(wrapCompare(theta,thetaAimHigh,midpointCompare)) { // rotate right
      m_green(ON);
      driveRightMotor(true,MOTOR_SPEED);
      driveLeftMotor(false,MOTOR_SPEED);
    } else if(wrapCompare(theta,thetaAimLow,midpointCompare)) { // rotate left
      m_green(OFF);
      driveRightMotor(false,MOTOR_SPEED);
      driveLeftMotor(true,MOTOR_SPEED);
    } else {
      m_green(ON);
      driveLeftMotor(true,MOTOR_SPEED);
      driveRightMotor(true,MOTOR_SPEED);
    }
  } else { // if we're in the proper xy range to push
    double angleThresh = angleRange(otherTheta+M_PI);
    // if angle is correct
    if(wrapCompare(theta,otherTheta+TRAIN_THETA_PAD,angleThresh)) {
      rotateRight();
      m_green(OFF);
    } else if(wrapCompare(theta,otherTheta-TRAIN_THETA_PAD,angleThresh)) {
      rotateLeft();
      m_green(OFF);
    } else {
      m_green(ON);
      driveLeftMotor(true,MOTOR_SPEED);
      driveRightMotor(true,MOTOR_SPEED);
    }
  }
}