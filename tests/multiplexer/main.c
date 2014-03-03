/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "saast.h"

volatile int val[]  = {0,0,0,0,0,0,0,0};
int cBin[] = {0,0,0,0,1,1,1,1};
int bBin[] = {0,0,1,1,0,0,1,1};
int aBin[] = {0,1,0,1,0,1,0,1};
volatile int i;

int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_init();
		m_usb_init();
		
		
    /* insert your hardware initialization here */
    for(;;){
        /* insert your main loop code here */
		for(i = 0; i<=7; i++)
		{
		m_gpio_out(F7, cBin[i]);	//Selevt
		m_gpio_out(F6, bBin[i]);	//Select
		m_gpio_out(F1, aBin[i]);	//Select
		
		m_gpio_out(F4, ON);			//Strobe on
		val[i] = m_gpio_in(C7);		//Get value
		m_gpio_out(F4, OFF);		//Strobe off
		m_usb_tx_int(val[i]);
		
		}
		//m_wait(100);
		m_usb_tx_string("\r\n");
    }
    return 0;   /* never reached */
}
