/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "saast.h"

volatile int sequence = 0;
volatile int record = 0;
volatile int count = 0;

const int check = 10000;
const int intSize = 32767;
const int multiplier = 1;

ISR(TIMER3_CAPT_vect)
{
	if(check(TCCR3B, ICES3) == 1)
	{
		record = ICR3;
		set(TIFR3, TOV3);		//clear overflow
		clear(TCCR3B, ICES3);	//Switch to falling edge
		//m_red(ON);
	}
	else
	{
		record = ICR3 - record;
		set(TCCR3B, ICES3);		//Switch to rising edge
		//m_red(OFF);
		sequence = 0;
	}
	set(TIFR3, ICF3);			//Clear input capture flag
}

int inputCaptureEnable(void)	//Set C7 to measure pulsewidth
{
	sequence = 1;
	sei();					//enable interrupts
	DDRC &= 0x7F;			//Clear bit 7 to make C7 input
	set(TCCR3B, ICES3);		//Set ICES3 for rising edge
	set(TIMSK3, ICIE3); 	//Set ICIE3 for input capture interrupt
	TCCR3B |= 0x01;			//Start Timer3 with a prescalar of 1  
}

int startPulse(void)			//Output start pulse
{
	DDRC |= 0x80;			//Set bit 7 to make C7 output
	m_gpio_out(C7, OFF);
	m_wait(2);
	m_gpio_out(C7, ON);
	m_wait(5);
	m_gpio_out(C7, OFF);
	count++;
}

int sensorOne()
{
	m_gpio_out(F7, OFF);
	m_gpio_out(F6, OFF);
	startPulse();
	inputCaptureEnable();
}

int sensorTwo()
{
	m_gpio_out(F7, ON);
	m_gpio_out(F6, OFF);
	startPulse();
	inputCaptureEnable();
}

int sensorThree()
{
	m_gpio_out(F7, OFF);
	m_gpio_out(F6, ON);
	startPulse();
	inputCaptureEnable();
}

int pingSensors()
{
		if(sequence == 0)
        {
			
			if(record < 0)
			{
				record = intSize;
			}
			
			if(count <= 1*multiplier) 
			{
				sensorOne();
				m_wait(10);
				if(record < 0)
				{
				record = intSize;
				}
				if(record < check)
				{
					m_green(ON);
					m_gpio_out(F4, ON);
					m_gpio_out(D6, OFF);
					m_gpio_out(D5, OFF);
					//m_usb_tx_int(record);
					//m_usb_tx_string("-1\r\n");	
				}
				else if(record > check)
				{
					m_green(OFF);
					//m_usb_tx_int(record);
					//m_usb_tx_string("-2\r\n");
					//m_wait(100);
				}
			}
			if(count <= 2*multiplier && count > 1*multiplier)
			{
				sensorTwo();
				m_wait(10);
				if(record < 0)
				{
				record = intSize;
				}
				if(record < check)
				{
					m_red(ON);
					m_gpio_out(F4, OFF);
					m_gpio_out(D6, ON);
					m_gpio_out(D5, OFF);	
				}
				else if(record > check)
				{
					m_red(OFF);
				}
			}
			if(count <= 3*multiplier && count > 2*multiplier)
			{
				sensorThree();
				m_wait(10);
				if(record < 0)
				{
				record = intSize;
				}
				if(record < check)
				{
					m_gpio_out(F4, OFF);
					m_gpio_out(D6, OFF);
					m_gpio_out(D5, ON);	
				}
				
			}
			if(count > 3*multiplier)
			{
				count = 0;
			}
		}
}

int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_init();
		//m_usb_init();
		m_green(ON);
		ICR3 = 0x0000;
		
		//Switches for MUX/DEMUX
		m_gpio_out(F6, OFF);
		m_gpio_out(F7, OFF);
				
		//Switches for H-Bridge		
		//m_gpio_out(F4, ON);
		m_gpio_out(D3, OFF);
		m_gpio_out(F5, ON);
		
		m_gpio_out(D4, ON);
		m_gpio_out(D5, OFF);
		
		m_gpio_out(D6, ON);
		m_gpio_out(F4, OFF);
	
		m_pwm_timer(1, 20000.0);
		//m_pwm_timer(3, 20.0);
		m_pwm_timer(4, 15000.0);
		m_pwm_output(1, 1, ON);
		m_pwm_output(1, 2, ON);
		m_pwm_output(4, 2, ON);
		m_pwm_output(4, 3, ON);
		//m_pwm_output(3, 1, ON);
		m_pwm_output(4, 2, ON);
		m_pwm_duty(1, 1, 90.0); //B6 Operates between 60-99
		m_pwm_duty(1, 2, 90.0);	//Motor B7
		m_pwm_duty(4, 2, 5.5);	//LED Voltage booster
		m_pwm_duty(4, 3, 90.0);	//Motor D7
		
		//Switches for Music
		m_gpio_out(B0, ON);
		m_gpio_out(B1, ON);
		m_gpio_out(B2, ON);
		m_gpio_out(B3, ON);
		
    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
        while(1)
        {
			//pingSensors();
			//m_pwm_output(3, 1, ON);
			
			m_wait(5000);
			m_gpio_out(B1, OFF);
			
			
			m_pwm_duty(1, 1, 0.0);
			m_pwm_duty(1, 2, 0.0);
			m_pwm_duty(4, 3, 0.0);
			m_gpio_out(D3, OFF);
			m_gpio_out(F5, ON);
			m_gpio_out(D4, OFF);
			m_gpio_out(D5, ON);
			m_gpio_out(D6, OFF);
			m_gpio_out(F4, ON);	
			m_wait(400);
			m_pwm_duty(1, 1, 90.0);
			m_pwm_duty(1, 2, 90.0);
			m_pwm_duty(4, 3, 90.0);
			m_wait(5000);
			m_pwm_duty(1, 1, 0.0);
			m_pwm_duty(1, 2, 0.0);
			m_pwm_duty(4, 3, 0.0);
			m_gpio_out(D3, ON);
			m_gpio_out(F5, OFF);
			m_gpio_out(D4, ON);
			m_gpio_out(D5, OFF);
			m_gpio_out(D6, ON);
			m_gpio_out(F4, OFF);
			m_wait(400);
			m_pwm_duty(1, 1, 90.0);
			m_pwm_duty(1, 2, 90.0);
			m_pwm_duty(4, 3, 90.0);
			
		}
    }
    return 0;   /* never reached */
}
