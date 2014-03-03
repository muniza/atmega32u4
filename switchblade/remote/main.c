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
#define PACKET_LENGTH 13
#define ATOI_LENGTH 10
#define ADC_LENGTH 5

char buffer[PACKET_LENGTH];
char buffer1[ADC_LENGTH];
char buffer2[ADC_LENGTH];
char num1[ADC_LENGTH];	// Need 5 for itoa and atoi
char num2[ADC_LENGTH]; 
char button1;
char button2;
char button3;
volatile int adc1 = 0;
volatile int adc2 = 0;
volatile int test1;
volatile int test2;
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
		m_usb_init();
		m_rf_open(CHANNEL, RXADDRESS, PACKET_LENGTH);
		
    /* insert your hardware initialization here */
		
		
        /* insert your main loop code here */
        while(1)
        {
			//adc = m_adc(F7);
			itoa(m_adc(F7), num1, 10);
			itoa(m_adc(F6), num2, 10);
			
			for(i=0; i<(ATOI_LENGTH-1); i++)
			{
				if(i < ADC_LENGTH)
				{
					buffer1[i] = num1[i];
				}
				else
				{
					buffer2[i-ADC_LENGTH] = num2[i-ADC_LENGTH];
				}
			}
			
			for(i=0; i<(ATOI_LENGTH-1); i++)
			{
				if(i < ADC_LENGTH)
				{
					buffer[i] = buffer1[i];
				}
				else
				{
					buffer[i] = buffer2[i-ADC_LENGTH];
				}
			}
			
			button1 = (char)((m_gpio_in(F5)+48));
			button2 = (char)(m_gpio_in(F4)+48);
			button3 = (char)(m_gpio_in(F1)+48);
			buffer[10] = button1;
			buffer[11] = button2;
			buffer[12] = button3;
			
			/*
			test1 = atoi(buffer1);
			test2 = atoi(buffer2);
			m_usb_tx_int(m_adc(F7));
			m_usb_tx_string("\r\n test2 - ");
			m_usb_tx_int(m_adc(F6));
			m_usb_tx_string("\r\n button1 - ");
			m_usb_tx_int(button1);
			m_usb_tx_string("\r\n button2 - ");
			m_usb_tx_int(button2);
			m_usb_tx_string("\r\n button3 - ");
			m_usb_tx_int(button3);
			m_usb_tx_string("\r\n test1 - ");
			*/
			
			m_red(TOGGLE);
			m_wait(50);
			m_rf_send(TXADDRESS, buffer, PACKET_LENGTH);
		}
}  
