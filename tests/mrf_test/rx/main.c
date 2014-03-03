/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "saast.h"

#define CHANNEL 1
#define RXADDRESS 0xA3
#define TXADDRESS 0xD9
#define PACKET_LENGTH 10
#define ADC_LENGTH 5

char buffer[PACKET_LENGTH];
char test1[ADC_LENGTH];
char test2[ADC_LENGTH];
volatile int num1;
volatile int num2;
volatile int i;

ISR(INT2_vect) {
	m_rf_read(buffer, PACKET_LENGTH);
	for(i=0; i<(PACKET_LENGTH); i++)	
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
	m_green(TOGGLE);
}

int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_init();
		m_bus_init();
		//m_usb_init();
		m_rf_open(CHANNEL, RXADDRESS, PACKET_LENGTH);
		m_gpio_out(F1, OFF);
		

    /* insert your hardware initialization here */
		
		
        /* insert your main loop code here */
        while(1)
        {
				//m_rf_send(TXADDRESS, buffer, PACKET_LENGTH);
				num1 = atoi(test1);
				num2 = atoi(test2);
				
				if(num1 == 1023)
				{
					m_gpio_out(F1, ON);
				}
				if(num1 == 0) 
				{
					m_gpio_out(F1, OFF);
				}
				if(num2 == 1023)
				{
					m_gpio_out(F4, ON);
				}
				if(num2 == 0)
				{
					m_gpio_out(F4, OFF);
				}
				//m_usb_tx_char(test);
		}
}
    
