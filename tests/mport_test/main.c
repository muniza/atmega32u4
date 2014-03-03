/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_port.h"

unsigned char address = 0x20;

int one(void)
{
	m_bus_init();
	m_port_init(address);

	m_port_set(address, DDRG, 0);
	m_port_set(address, DDRG, 1);
	m_port_set(address, DDRG, 2);
	m_port_set(address, DDRG, 3);
	m_port_set(address, DDRG, 4);
	m_port_set(address, DDRG, 5);
	m_port_set(address, DDRG, 6);
	m_port_set(address, DDRG, 7);

	m_port_set(address, DDRH, 0);
	m_port_set(address, DDRH, 1);
	m_port_set(address, DDRH, 2);
	m_port_set(address, DDRH, 3);
	m_port_set(address, DDRH, 4);
	m_port_set(address, DDRH, 5);
	m_port_set(address, DDRH, 6);
	m_port_set(address, DDRH, 7);

	m_port_set(address, PORTH, 0);
	m_port_set(address, PORTH, 1);
	m_port_set(address, PORTH, 2);
	m_port_set(address, PORTH, 3);
	m_port_set(address, PORTH, 4);
	m_port_set(address, PORTH, 5);
	m_port_set(address, PORTH, 6);
	m_port_set(address, PORTH, 7);

	m_port_set(address, PORTG, 0);
	m_port_set(address, PORTG, 1);
	m_port_set(address, PORTG, 2);
	m_port_set(address, PORTG, 3);
	m_port_set(address, PORTG, 4);
	m_port_set(address, PORTG, 5);
	m_port_set(address, PORTG, 6);
	m_port_set(address, PORTG, 7);
	
	return 0;
}

int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_clockdivide(0);
		m_disableJTAG();
		one();
		set(DDRD, 2);

			
    /* insert your hardware initialization here */
    while(1) 
    {
		
		m_wait(1000);
		set(PORTD, 2);
		m_port_clear(address, PORTH, 0);
		m_port_clear(address, PORTH, 1);
		m_port_clear(address, PORTH, 2);
		m_port_clear(address, PORTH, 3);
		m_port_clear(address, PORTH, 4);
		m_port_clear(address, PORTH, 5);
		m_port_clear(address, PORTH, 6);
		m_port_clear(address, PORTH, 7);

		m_port_clear(address, PORTG, 0);
		m_port_clear(address, PORTG, 1);
		m_port_clear(address, PORTG, 2);
		m_port_clear(address, PORTG, 3);
		m_port_clear(address, PORTG, 4);
		m_port_clear(address, PORTG, 5);
		m_port_clear(address, PORTG, 6);
		m_port_clear(address, PORTG, 7);

		m_green(TOGGLE);
		
		m_wait(1000);
		clear(PORTD, 2);
		
		m_port_set(address, PORTH, 0);
		m_port_set(address, PORTH, 1);
		m_port_set(address, PORTH, 2);
		m_port_set(address, PORTH, 3);
		m_port_set(address, PORTH, 4);
		m_port_set(address, PORTH, 5);
		m_port_set(address, PORTH, 6);
		m_port_set(address, PORTH, 7);
	
		m_port_set(address, PORTG, 0);
		m_port_set(address, PORTG, 1);
		m_port_set(address, PORTG, 2);
		m_port_set(address, PORTG, 3);
		m_port_set(address, PORTG, 4);
		m_port_set(address, PORTG, 5);
		m_port_set(address, PORTG, 6);
		m_port_set(address, PORTG, 7);
		
	}
    return 0;   /* never reached */
}
