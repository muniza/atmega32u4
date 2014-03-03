/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "m_general.h"

int setupOne = 1;

int one(void)
{
	if(setupOne)
	{
		set(DDRD, 7);
		set(DDRD, 6);
		set(DDRD, 5);
		set(DDRD, 4);
		
		clear(PORTD, 7);
		
		setupOne = 0;
	}
	
	return 0;
}

int blue(void)
{
	clear(PORTD, 6);
	set(PORTD, 5);
	clear(PORTD, 4);
	
	return 0;
}

int cyan(void)
{
	set(PORTD, 6);
	set(PORTD, 5);
	clear(PORTD, 4);
	
	return 0;
}

int green(void)
{
	set(PORTD, 6);
	clear(PORTD, 5);
	clear(PORTD, 4);
	
	return 0;
}

int yellow(void)
{
	set(PORTD, 6);
	clear(PORTD, 5);
	set(PORTD, 4);
	
	return 0;
}

int red(void)
{
	clear(PORTD, 6);
	clear(PORTD, 5);
	set(PORTD, 4);
	
	return 0;
}

int magenta(void)
{
	clear(PORTD, 6);
	set(PORTD, 5);
	set(PORTD, 4);
	
	return 0;
}

int white(void)
{
	set(PORTD, 6);
	set(PORTD, 5);
	set(PORTD, 4);
	
	return 0;
}


int main(void)
 {
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_clockdivide(0);
		one();
			
    /* insert your hardware initialization here */
    while(1) 
    {
		white();
		m_wait(1000);
		magenta();
		m_wait(1000);
		red();
		m_wait(1000);
		yellow();
		m_wait(1000);
		green();
		m_wait(1000);
		cyan();
		m_wait(1000);
		blue();
		m_wait(1000);
	}
    return 0;   /* never reached */
}
