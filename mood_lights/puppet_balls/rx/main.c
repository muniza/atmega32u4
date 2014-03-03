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
#include <avr/wdt.h> 

#define CHANNEL 2
#define RXADDRESS 0xA1
#define TXADDRESS 0xA2
#define PACKET_LENGTH 3

int sync = 0;
int receive = 0;
int i = 0;
int dead_count = 20;

float blue = 99.0;
float green = 99.0;
float red = 99.0;
float speed = 0.1;
float conversion = 33/85;

// bluex greenx redx
char buffer[PACKET_LENGTH] = {0, 0, 0};
int threshold;

//When Packet Arrives get ready to play
ISR(INT2_vect) 
{
	m_green(TOGGLE);
	receive = 1;
	wdt_reset();
}

int setup_pwm(void)
{	
	set(DDRB,4);
	set(DDRB,5);
	set(DDRB,6);
	set(DDRC,6);
	
	set(PORTB,4); //Clear = ON, Set = OFF
	
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
	m_pwm_duty(1, 1, green);
	m_pwm_duty(3, 1, red);
	m_pwm_duty(4, 2, blue);
	
	return 0;
}


int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		
		sei();
		m_clockdivide(0);
		setup_pwm();
		m_bus_init();
		
		wdt_reset();
		wdt_enable(WDTO_30MS);
		
		while(!m_rf_open(CHANNEL, RXADDRESS, PACKET_LENGTH)){}
		
        while(1)
        {	
			if(dead_count == 0)
			{
				set(PORTB, 4); //off
				receive = 0;
				sync = 0;
				m_red(TOGGLE);
			}
			dead_count--;
			if(dead_count < 0) dead_count=0;
			
			if(receive)
			{
				m_rf_read(buffer, PACKET_LENGTH);
				blue = (int)buffer[0];
				green = (int)buffer[1];
				red = (int)buffer[2];
				
				sync = 1;
				dead_count = 20;
				clear(PORTB, 4);
			}
			if(sync)
			{
				dim();
			}
		}
} 
