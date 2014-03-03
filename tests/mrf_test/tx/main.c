/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "saast.h"

#define CHANNEL 1
#define RXADDRESS 0xD9
#define TXADDRESS 0xA3
#define PACKET_LENGTH 10
#define ADC_LENGTH 5

char buffer[PACKET_LENGTH];
char num1[ADC_LENGTH];	// Need 5 for itoa and atoi
char num2[ADC_LENGTH]; 
volatile int adc1 = 0;
volatile int adc2 = 0;
volatile int test;
volatile int i;

ISR(INT2_vect) {
	//m_rf_read(buffer, PACKET_LENGTH);
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
		m_gpio_out(F4, OFF);

		
    /* insert your hardware initialization here */
		
		
        /* insert your main loop code here */
        while(1)
        {
			//adc = m_adc(F7);
			itoa(m_adc(F7), num1, 10);
			itoa(m_adc(F6), num2, 10);
			for(i=0; i<(PACKET_LENGTH); i++)
			{
				if(i < ADC_LENGTH)
				{
					buffer[i] = num1[i];
				}
				else
				{
					buffer[i] = num2[i-ADC_LENGTH];
				}
			}
			/*
			m_usb_tx_char(num[0]);
			m_usb_tx_char(num[1]);
			m_usb_tx_char(num[2]);
			m_usb_tx_char(num[3]);
			
			test = atoi(buffer);
			m_usb_tx_int(test);
			m_usb_tx_string("\r\n");
			*/
			m_wait(50);
			m_rf_send(TXADDRESS, buffer, PACKET_LENGTH);
		}
}  
