//
//  main.c
// wireless transmitter
//
//  Created by Kevin Bui on 12/2/16.
//
//

#include "m_general.h"
#include <math.h>
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include <stdio.h>
#include <stdlib.h>
#define channel 1
#define addressTx 40 //three addresses are 64, 65, 66
#define addressRx 40
#define packLen 10
#define blueLED 7
#define redLED 6

//global vars
char buffer[packLen] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned int wiiBuffer[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
unsigned char transmitVal = 0;
int puckThreshold = 500;
unsigned int PWM_L = 26135;
unsigned int PWM_R = 45535;
double difLimit = 500;

void init();
void initTimer1();
void initTimer3();
void initTimer4();
void localize();
void flashBlueLED();
void flashRedLED();
void turnOnBlueLED();
void turnOffBlueLED();
void turnOnRedLED();
void turnOffRedLED();

int main(void)
{
    init();
    m_green(ON);
    m_wait(100);
    while(!m_usb_isconnected());
    m_green(OFF);
    //flashBlueLED();
    m_usb_rx_flush();
  while(1)
  {
    //m_usb_tx_string("in loop \n\r");
    if(m_usb_rx_available()) {

      //m_usb_tx_string("trasmitVal = ");
      transmitVal = m_usb_rx_char();
     m_usb_tx_int((int)transmitVal);
      m_usb_tx_string("\n\r");
      if(transmitVal == '1') transmitVal = 0xA0;
      if(transmitVal == '2') transmitVal = 0xA1;
      if(transmitVal == '3') transmitVal = 0xA4;
      if(transmitVal == '4') transmitVal = 0xA6;
      if(transmitVal == '9')
      {
        transmitVal = 0xA8;
        difLimit+= 10;
        m_usb_tx_string("dif limit = ");
        m_usb_tx_int((int)difLimit);
        m_usb_tx_string("\n\r");
      } 
      if(transmitVal == '0')
      {
        transmitVal = 0xA9;
        difLimit-= 10;
        m_usb_tx_string("dif limit = ");
        m_usb_tx_int((int)difLimit);
        m_usb_tx_string("\n\r");        
      } 
      //if(transmitVal == '9') PWM_L += 100;
      //if(transmitVal == '0') PWM_L -= 100;
      buffer[0] = transmitVal;
      m_rf_send(addressTx, buffer, packLen);
      

    //m_wait(100);    
    
    //m_wait(100);

  }

    
}
return 0;
}


void init()
{
  m_clockdivide(1);
  m_bus_init();
  m_usb_init(); //initialize usb communication
  initTimer1(); //initialize timer1
  m_rf_open(channel, addressRx, packLen); //initialize RF protocol
  sei();
  //m_usb_tx_string("init \n\r");

}

//initialize Timer 1 for frequency control
void initTimer1(void)
{
  set(PORTB, 6);
  set(DDRB, 6);
    //set Timer 1 prescaler to 1
    clear(TCCR1B, CS12);      //CS12 = 1;
    clear(TCCR1B, CS11);    //CS11 = 0;
    set(TCCR1B, CS10);    //CS10 = 0;

    //set timer mode 15
    set(TCCR1B, WGM13);   //WGM13 = 1
    set(TCCR1B, WGM12);     //WGM12 = 1
    set(TCCR1A, WGM11);   //WGM11 = 1
    set(TCCR1A, WGM10);   //WGM10 = 1
    OCR1A = 65535;     //15625
    OCR1B = 65535;     // initialize to 100% duty cycle   

    //set channel B compare output response clear at rollover
    set(TCCR1A, COM1B1);  //COM1B1 = 1
    set(TCCR1A, COM1B0);    //COM1B0 = 1
    set(TIMSK1, TOIE1);
    //set(TIMSK1, OCIE1A);
}

//output pin C6
void initTimer3(void)
{
  //set Timer 3 prescaler to 8
  clear(TCCR3B, CS32);
  set(TCCR3B, CS31);
  clear(TCCR3B, CS30);
  //set timer mode 0
  clear(TCCR3B, WGM33);
  clear(TCCR3B, WGM32);
  clear(TCCR3A, WGM31);
  clear(TCCR3A, WGM30);
  //OCR3A = 1000;
  //interrupt
  //set(TIMSK3, OCIE3A);
}


//output pins D7 and and complement D6
void initTimer4(void)
{
    set(DDRD, 7);
    set(DDRD, 6);
    //prescaler of 1
    clear(TCCR4B, CS43);
    clear(TCCR4B, CS42);
    clear(TCCR4B, CS41);
    set(TCCR4B, CS40);
    OCR4C = 255;
    //Channel D set at comp with OCR4A
    clear(TCCR4C, PWM4D);
    set(TCCR4C, COM4D1);
    set(TCCR4C, COM4D0);
    OCR4D = 100;
}


// flash blue LED
void flashBlueLED(void)
{
  turnOffBlueLED();
  m_wait(40);
  turnOnBlueLED();
  m_wait(40);
  turnOffBlueLED();
}

void turnOnBlueLED(void)
{
  set(PORTD, blueLED);
}

void turnOffBlueLED(void)
{
  clear(PORTD, blueLED);
}

void flashRedLED(void)
{
  turnOnRedLED();
  m_wait(40);
  turnOffRedLED();
}

void turnOnRedLED(void)
{
  set(PORTD, redLED);
}

void turnOffRedLED(void)
{
  clear(PORTD, redLED);
}

//read IMU values and adjust pwm output (OCR1B)
ISR(TIMER1_OVF_vect)
{
//read IMU vals;
//control calculations;
//update OCR1B;
}


//turn sound off after OCR3A is reached
ISR(TIMER3_COMPA_vect)
{
  //m_usb_tx_string("compa \n\r");
  //clear(DDRB, 6);
  //clear(TIMSK3, OCIE3A);  
}

//interrupt when receives packet

ISR(INT2_vect)
{
  m_rf_read(buffer, packLen);
  char turnLeftThreshold = buffer[0];
  char turnRightThreshold = buffer[1];
 /*
  m_usb_tx_string("turnLeftThreshold = ");
  m_usb_tx_int((int)turnLeftThreshold);
  m_usb_tx_string(" turnRightThreshold = ");
  m_usb_tx_int((int)turnRightThreshold);
  m_usb_tx_string(" puckThreshold = ");
  m_usb_tx_int(puckThreshold);
  
  if (turnLeftThreshold == 1) PWM_R += 200;
  if(turnLeftThreshold == 0) PWM_R -= 200;
  m_usb_tx_string("PWM_R = ");
  m_usb_tx_uint(PWM_R);
  m_usb_tx_string("\n\r");
*/
}


