/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 * 
 */

/*Two Modes
 * Demo Mode is locked
 * Drive Mode starts with vroom. User controllable lights and movement
 */

#include <avr/io.h>
#include "saast.h"

//Wireless
#define CHANNEL 1
#define RXADDRESS 0xA3
#define TXADDRESS 0xD9
#define PACKET_LENGTH 13
#define ATOI_LENGTH 10
#define ADC_LENGTH 5

char buffer[PACKET_LENGTH];
char test1[ADC_LENGTH];
char test2[ADC_LENGTH];
char button1;
char last1;
char button2;
char last2;
char button3;
char last3;
volatile int num1;
volatile int num2;
volatile int i;

//Initilizers
int initBoost = 0;
int initShowOff = 0;
int showOffNow = 0;
int initDrive = 0;
int driveNow = 0;
int lightsNow = 0;
int lightsOff = 0;
float duty = 0.0;

ISR(INT2_vect) {
	m_rf_read(buffer, PACKET_LENGTH);
	m_green(TOGGLE);
}

int wireless()
{
	for(i=0; i<(ATOI_LENGTH); i++)	
	{
		if(i < ADC_LENGTH)
		{
			test1[i]=buffer[i];
		}
		else
		{
			test2[i-ADC_LENGTH] = buffer[i];
		}
	}
	
	num1 = atoi(test1);
	num2 = atoi(test2);
	
	//Demo Mode
	button1 = buffer[10];
	if(button1 == '1' && last1 == '0')
	{			
		if(showOffNow == 0)
		{
			showOffNow = 1;
		}
		
		//Get out of ShowOff Mode
		if(showOffNow == 2)
		{
			showOffNow = 0;
		}
		last1 = '1';
	}
	if(button1 == '0')
	{
		last1 = '0';
	}
	
	//Drive Mode
	button2 = buffer[11];
	if(button2 == '1' && last2 == '0')
	{
		if(driveNow == 0)
		{
			driveNow = 1;
		}
		//Get out of Drive Mode
		if(driveNow == 2)
		{
			driveNow = 0;
		}
		last2 = '1';
	}
	if(button2 == '0')
	{
		last2 = '0';
	}
	
	//Lights
	button3 = buffer[12];
	if(button3 == '1' && last3 == '0')
	{
		//Dim Lights
		if(lightsNow == 0 && lightsOff)
		{
			m_pwm_duty(4,1,30);
			lightsNow = 1;
			lightsOff = 0;
		}
		
		//Bright Lights
		if(lightsNow == 1 && lightsOff)
		{
			m_pwm_duty(4,1,90);
			lightsNow = 2;
			lightsOff = 0;
		}
		
		//Lights Off
		if(lightsNow == 2 && lightsOff)
		{
			m_pwm_duty(4,1,0);
			lightsNow  = 0;
			lightsOff = 0;
		}
		lightsOff = 1;
		last3 = '1';
	}
	if(button3 == '0')
	{
		last3 = '0';
	}
	//Test Drive
	m_pwm_duty(3, 1, 10.0);                    
	m_gpio_out(D7, OFF);
	
	
	return 0;
}

int showOff()
{
	if(initShowOff == 0)
	{
		m_gpio_out(B5, OFF);
		m_gpio_out(F6, OFF);
		m_gpio_out(F0, OFF);
	}
	
	//Open Doors
	m_gpio_out(B5, OFF);
	m_gpio_out(F6, ON);
	m_wait(1800);
	
	//Hold Doors Open
	m_gpio_out(B5, OFF);
	m_gpio_out(F6, OFF);
	
	//Send Play Music Command
	m_gpio_out(F0, ON);
	m_wait(6000);
	m_gpio_out(F0, OFF);
	
	//Close Doors
	m_gpio_out(B5, ON);
	m_gpio_out(F6, OFF);
	m_wait(1800);
	
	//Hold Doors Closed
	m_gpio_out(B5, OFF);
	m_gpio_out(F6, OFF);
	m_wait(10000);
	
	return 0;
}

int boost()
{
   if(initBoost == 0)
   {
		m_pwm_duty(1, 1, 20);
		initBoost = 1;
	}
	
	return 0;
}

int drive()
{
	if(num1 > 512)
	{
		duty = 0.2*num1 - 102.4;
		clear(PIND, 6); 
	}
	else
	{
		duty = -0.2*num1 + 102.4;
		set(PIND, 6);  
	}
	
	if(duty > 99)
	{
		duty = 99;
	}
	m_pwm_duty(3, 1, duty); 
	
	return 0;
}

int main(void)
{
	DDRE |= 1<<6;
	PORTE &= !(1<<6);
	
   //Set Clock, Disable JTAG, Open Wireless 
	m_init();
	m_bus_init();
	m_clockdivide(0);
	m_rf_open(CHANNEL, RXADDRESS, PACKET_LENGTH);
	
	m_gpio_out(F0, OFF);
		
	//Setup Timers for PWM	
	m_pwm_timer(1, 20000.0);
	m_pwm_timer(3, 18000.0);
	m_pwm_timer(4, 10000.0);
		
	m_pwm_output(1, 2, ON);	//pin B7 for steering
	m_pwm_output(1, 1, ON);	//B6 for relay
	m_pwm_duty(1, 1, 10);
	//m_pwm_output(1, 2, 2.0);
	m_pwm_output(3, 1, ON);	//pin C6 drive motors
	m_pwm_duty(3, 1, 0.0);                    
	//m_gpio_out(D7, OFF);
	m_pwm_output(4, 1, ON);	//pin C7 for lights
	m_pwm_duty(4,1, 0.0);
	m_pwm_output(4, 3, ON); //D7 for switch
		
	boost();
	
	clear(DDRD, 7);
	set(DDRD, 6);
	
	while(1)
	{ 
		wireless();
		m_red(TOGGLE);
		
		if(showOffNow)
		{
			showOff();
			showOffNow = 0;
		}
		
		if(driveNow)
		{
			//drive();
		}
	}
    
	return 0;   /* never reached */
}
