/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

/*	1 master 3 slaves
 *  if on, turn the rest on
 *  if the others off, turn off
 * 
 * transition from color to color 
 *  sync to color change
 * 	
 * 	When on
 * 		look for messages
 * 		order thyself 1 2 3 4
 * 		broadcase signal sync {1 1 1 1}
 * 		color rainbow
 * 	PWM b5 b6 C6
 * 	switch b4 
 */ 

#include <avr/io.h>
#include "saast.h"

#define CHANNEL 2
#define RXADDRESS 0xA2
#define TXADDRESS 0xA1
#define PACKET_LENGTH 3

int send = 0;
int send_rate = 10;
int i = 0;
int threshold;

float blue = 1.0;
float green = 1.0;
float red = 99.0;
float speed = 0.1;

float conversion = 85/33;

// bluex greenx redx
char buffer[PACKET_LENGTH] = {0, 0, 0};
int threshold;

int setup_pwm(void)
{	
	set(DDRB,4);
	set(DDRB,5);
	set(DDRB,6);
	set(DDRC,6);
	
	clear(PORTB,4); //Clear = ON, Set = OFF
	
	m_pwm_timer(1, 15000);
	m_pwm_output(1, 1, ON);
	m_pwm_timer(3, 15000);
	m_pwm_output(3, 1, ON);
	m_pwm_timer(4, 15000);
	m_pwm_output(4, 2, ON);
	
	threshold = 99/speed;
	// 4 2 B5
	// 1 1 B6 
	// 3 1 C6
	
	return 0;
}

int dim(void)
{
	i += 1;      // Increment counter
	if (i < threshold) // First phase of fades
	{
		red   -= speed; // Red down
	    green += speed; // Green up
	    blue   = speed; // Blue low
	}
	else if (i < 2*threshold) // Second phase of fades
	{
	    red    = speed; // Red low
	    green -= speed; // Green down
	    blue  += speed; // Blue up
	} 
	else if (i < 3*threshold) // Third phase of fades
	{
	    red  += speed; // Red up
	    green = speed; // Green low
	    blue -= speed; // Blue down
	}
	else // Re-set the counter, and start the fades again
	{
	    i = 1;
	}  
	
	if(i%send_rate == 1)
	{
		m_red(TOGGLE);
		m_rf_send(TXADDRESS, buffer, PACKET_LENGTH);
		buffer[0] = (char)(blue);
		buffer[1] = (char)(green);
		buffer[2] = (char)(red);
		
	}
	else
	{
		send = 0;
	}
	
	m_pwm_duty(1, 1, green);
	m_pwm_duty(3, 1, red);
	m_pwm_duty(4, 2, blue);
	
	return 0;
}

int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		
		m_clockdivide(0);
		setup_pwm();
		m_bus_init();
		while(!m_rf_open(CHANNEL, RXADDRESS, PACKET_LENGTH)){}
		
        while(1)
        {	
			dim();
		}
} 
