/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"

#define CHANNEL 1
#define RXADDRESS 0x1A
#define PACKET_LENGTH 3

char buffer[PACKET_LENGTH] = {0,0,0};
int frequency = 300;
int frequencyL = 0;
int frequencyH = 0;
int duration = 0;
int usbInit = 0;
int initialize1 = 0;
int initialize2 = 0;
volatile int offset = 3.0;
volatile int play = 0;
volatile int holdReady = 0;
volatile int outputReady = 0;
volatile int i = 0;

//Don't want to create a seperate header file...
float sineTable[64] = {.5000,
.5490,
.5975,
.6451,
.6913,
.7357,
.7778,
.8172,
.8536,
.8865,
.9157,
.9410,
.9619,
.9785,
.9904,
.9976,
.9990,
.9976,
.9904,
.9785,
.9619,
.9410,
.9157,
.8865,
.8536,
.8172,
.7778,
.7357,
.6913,
.6451,
.5975,
.5490,
.5000,
.4510,
.4025,
.3549,
.3087,
.2643,
.2222,
.1828,
.1464,
.1135,
.0843,
.0590,
.0381,
.0215,
.0096,
.0024,
.0001,
.0024,
.0096,
.0215,
.0381,
.0590,
.0843,
.1135,
.1464,
.1828,
.2222,
.2643,
.3087,
.3549,
.4025,
.4510};

//Generate the sine wave frequency
ISR(TIMER1_COMPA_vect)
{	
	//Get the duty cycle for the sine wave
	OCR1B = sineTable[i]*OCR1A;
	i = offset+i;
	
	if(i > 63)
	{
		i = 0;
	}	
}

//When Packet Arrives get ready to play
ISR(INT2_vect) 
{
	//	m_rf_read(buffer, PACKET_LENGTH);

	m_green(TOGGLE);

	play = 1;
	holdReady = 1;
	outputReady = 1;
}

//Whenever timer matches duration
ISR(TIMER3_COMPA_vect)
{
	//Flag clears automatically
	m_red(TOGGLE);
	
	//Stop Sine Wave Generation after time finished
	clear(TCCR1A, COM1B1);
	set(TCCR1A, COM1B0);
	
	play = 0;
}

//Debug the wireless
int debug()
{
	if(usbInit == 0)
	{
		m_usb_init();
		usbInit = 1;
	}
	
	if(m_usb_isconnected())
	{
		m_usb_tx_string("\r\n Duration:  ");
		m_usb_tx_int((int)duration);
		m_usb_tx_string("\r\n FrequencyTotal:  ");
		m_usb_tx_int((int)frequency);
	}
	
	return 0;
}

int output()
{	

	if(initialize1 == 0)
	{
		//Clock 16MHz
		clear(TCCR1B, CS12);
		clear(TCCR1B, CS11);
		set(TCCR1B, CS10);
		
		//output
		set(DDRB, 6);
		
		//Output Compare
		set(TCCR1A, COM1B1);
		clear(TCCR1A, COM1B0);
		
		//Timer Mode 11
		set(TCCR1B, WGM13);
		clear(TCCR1B, WGM12);
		set(TCCR1A, WGM11);
		set(TCCR1A, WGM10);
		
		//Interrupt
		set(TIMSK1, OCIE1A);
		
		//30 Khz pulses
		OCR1A = 1000000/frequency;
		
		initialize1 = 1;
	}
	
	if(outputReady)
	{
		//Restart Output
		set(TCCR1A, COM1B1);
		clear(TCCR1A, COM1B0);
		
		frequencyL = (int)(buffer[0]);
		if(frequencyL < 0)
		{
			frequencyL = 127;
		}
		
		frequencyH = (int)(buffer[1]);
		if(frequencyH < 0)
		{
			frequencyH = 127;
		}
		
		//in DeciHertz
		frequency = (frequencyH*256+frequencyL)/10;
		//frequency = 300;
		OCR1A = 1000000/frequency;
		
		outputReady = 0;
	}
	
	//Fine Tuning on the scope
	offset = 8;
	
	return 1;
}


int hold()
{
	if(initialize2 == 0)
	{	
		//Start Timer3 with a prescalar of 1024
		set(TCCR3B, CS32);		
		clear(TCCR3B, CS31);
		set(TCCR3B, CS30);
		
		//Count up to OCR3A
		clear(TCCR3B, WGM33);
		set(TCCR3B, WGM32);
		clear(TCCR3A, WGM31);
		clear(TCCR3A, WGM30);
		
		//Setup Output Compare to do Timing
		clear(TCCR3A, COM3A1);
		set(TCCR3A, COM3A0);
		
		//Interrupt when reach OCR3A
		set(TIMSK3, OCIE3A);
		
		//Setup Output on C6
		set(DDRC, 6);
		
		initialize2 = 1; 
	}
	
	//holdReady = 1;
	//duration = 1000;
	
	if(holdReady)
	{
		//get duration
		duration = (int)(buffer[2]);
		
		if(duration < 0)
		{
			duration = 30;
		}
		
		//Equation from using the oscope and pin C6
		//duration = 60;
		
		OCR3A = (156*duration);//156.34-6.1714);
		holdReady = 0;
	}
	
	return 0;
}

int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_clockdivide(0);
		m_bus_init();
		m_rf_open(CHANNEL, RXADDRESS, PACKET_LENGTH);
		sei();
		/* insert your hardware initialization here */
		
        /* insert your main loop code here */
        while(1)
        {	
			//hold();
			if(play == 1)
			{
				//debug();
				m_rf_read(buffer, PACKET_LENGTH);
				output();
				hold();
			}
		}
}  
