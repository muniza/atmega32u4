/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 * 
 */

#include <avr/io.h>
#include "saast.h"

int done = 0;
int ready = 0;

const int clockPin = D0;  // the pin number of the clock pin
const int dataPin = D1;  // the pin number of the data pin
const int resetPin = F1;  // the pin number of the reset pin

const int HIGH = 1;
const int LOW = 0;

const unsigned int VOLUME_7 = 0xFFF7;

const unsigned int PLAY_PAUSE = 0xFFFE;
const unsigned int STOP = 0xFFFF;

unsigned int mask = 0x8000;

int setup() {
  m_gpio_out(clockPin, HIGH);
  m_gpio_out(dataPin, LOW);

  // reset the module
  m_gpio_out(resetPin, HIGH);
  m_wait(100);
  m_gpio_out(resetPin, LOW);
  m_wait(10);
  m_gpio_out(resetPin, HIGH);
  m_wait(100);

  sendCommand(VOLUME_7);
  m_green(ON);
}

int loop() {
  // play "0000.ad4"
  sendCommand(0x0000);
  m_wait(10000);
  
  // stop playing
  sendCommand(0xFFFF);
  m_wait(1000);
}

int sendCommand(unsigned int command) {
  // start bit
  m_gpio_out(clockPin, LOW);
  m_wait(2);

  // bit15, bit14, ... bit0
  for (mask = 0x8000; mask > 0; mask >>= 1) {
    if (command & mask) {
      m_gpio_out(dataPin, HIGH);
    }
    else {
      m_gpio_out(dataPin, LOW);
    }
    // clock low
    m_gpio_out(clockPin, LOW);
    m_wait(0.200);

    // clock high
    m_gpio_out(clockPin, HIGH);
    m_wait(0.200);
  }

  // stop bit
  m_wait(2);
}

int main(void)
{
	DDRE |= 1<<6;
	PORTE &= !(1<<6);
	
	//Set Clock and Disable JTAG 
	m_init();
	m_clockdivide(0);
	m_wait(200);	
	//Delay for Race		
	
	while(1)
	{
		ready = m_gpio_in(F0);
		
		if(ready == 1)
		{
			ready = 0;
			setup();
			loop();
		}
	}
    
	return 0;   /* never reached */
}
