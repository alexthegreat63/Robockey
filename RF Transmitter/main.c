/* Name: main.c
 * Authors: Alex Kearns, Nick Trivelis, Lincoln Talbott, Anirudh
 * kearnsa@seas.upenn.edu
 *
 * MEAM 510
 * Robockey Code
 * Controls robot for Robockey tournament
 */

#include <avr/io.h>
#include "m_rf.h"
#include "m_general.h"

//RF Parameters:
#define CHANNEL 1
#define ADDRESS 40 // address for robot 1
#define GOALIE_ADDRESS 42 // address for goalie
#define PACKET_LENGTH 10 // bytes

 // Packet contents
#define COMM 0xA0
#define PLAY 0xA1
#define PAUSE 0xA4
#define HALFTIME 0xA6
#define OVER 0xA7

char buffer[10] = {0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0};

// Comm Test 0xA0  0xA0  0xA0  0xA0  0xA0  0xA0  0xA0  0xA0  0xA0  0xA0  robots must flash the positioning LED
// Play  0xA1  0xA1  0xA1  0xA1  0xA1  0xA1  0xA1  0xA1  0xA1  0xA1  robots must illuminate the positioning LED and move in a noticeable way
// Goal R  0xA2  SA  SB  0 0 0 0 0 0 0 SR = score R, SB = score B
// Goal B  0xA3  SA  SB  0 0 0 0 0 0 0 SR = score R, SB = score B
// Pause 0xA4  0xA4  0xA4  0xA4  0xA4  0xA4  0xA4  0xA4  0xA4  0xA4  robots must stop within three seconds
// Halftime  0xA6  0xA6  0xA6  0xA6  0xA6  0xA6  0xA6  0xA6  0xA6  0xA6
// Game Over 0xA7  0xA7  0xA7  0xA7  0xA7  0xA7  0xA7  0xA7  0xA7  0xA7

int main(void) {
	m_clockdivide(0); // set system clock speed
	m_green(ON);
	m_rf_open(CHANNEL, ADDRESS, PACKET_LENGTH);
	m_green(OFF);
	// int i;
	// for(i = 0; i < 10; i++) {
	// 	buffer[i] = COMM;
	// }
	m_wait(5000);
	m_red(ON);
	m_rf_send(ADDRESS, buffer, PACKET_LENGTH);
	m_wait(5000);
	m_red(OFF);
  	while(1);
}