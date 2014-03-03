/***** MEAM510 ACROBAT TEAM 27 - Alfredo Muniz & Sade Oba *****/

#include <avr/io.h>
#include "m_general.h"
#include "m_imu.h"
#include "m_bus.h"
#include "m_usb.h"

unsigned char accel_scale = 0;
unsigned char gyro_scale = 0;
int data[9];
int debugInit = 1;
int readReady = 1;
int setPoint = 0;
int checkPoint = 0;

//If data is ready to be read, then you have permission to read it
int read(void)
{
	if(readReady){	
		m_imu_raw(data);
	
		readReady = 0;
	}
	return 0;
}

//Interrupt from the IMU used to tell when data is ready to be read
ISR(INT2_vect)
{
	readReady = 1;
}

//Sets up the directions and pwm for the motors
int setup(void)
{	
	set(DDRF, 7);
	set(DDRF, 6);
	
	set(PORTF, 6);
	set(PORTF, 7);
	
	OCR1A = 0x0180;	//20 kHz
	OCR1B = .5*OCR1A;
	OCR1C = .5*OCR1A;
	
	//Clock /1 
	clear(TCCR1B, CS12);
	clear(TCCR1B, CS11);
	set(TCCR1B, CS10);
	
	//output
	set(DDRB, 5);
	set(DDRB, 6);
	set(DDRB, 7);
	
	//Timer Mode 11
	set(TCCR1B, WGM13);
	clear(TCCR1B, WGM12);
	set(TCCR1A, WGM11);
	set(TCCR1A, WGM10);
	
	//Output Compare
	set(TCCR1A, COM1A1);
	set(TCCR1A, COM1A0);
	set(TCCR1A, COM1B1);
	set(TCCR1A, COM1B0);
	set(TCCR1A, COM1C1);
	set(TCCR1A, COM1C0);
	
	return 0;
}

//To figure out what the poop IMU is telling me
int debug(void)
{
	if(debugInit){
		m_usb_init();
		debugInit = 0;
	}
	
	if(m_usb_isconnected())
	{
		m_usb_tx_string("\r\n Ax:  ");
		m_usb_tx_int(data[0]);
		m_usb_tx_string("\r\n Ay:  ");
		m_usb_tx_int(data[1]+650);
		m_usb_tx_string("\r\n Az:  ");
		m_usb_tx_int(data[2]+9200);
		m_usb_tx_string("\r\n Gx:  ");
		m_usb_tx_int(data[3]);
		m_usb_tx_string("\r\n Gy:  ");
		m_usb_tx_int(data[4]);
		m_usb_tx_string("\r\n Gz:  ");
		m_usb_tx_int(data[5]);
	}
	
	return 0;
}

/*Our VERY BASIC controller that just switches direction when Ay is the 
 * Other way. This seems to work fine so no need to change it.
 */
int control(void)
{
	if(checkPoint > 0)
	{
		clear(PORTF, 7);
		clear(PORTF, 6);
		m_red(ON);
		m_green(OFF);
	}
	else
	{
		set(PORTF, 7);
		set(PORTF, 6);
		m_red(OFF);
		m_green(ON);
	}
	return 0;
}

int main(void)
{
		//Interrupts, F port, Speed
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_clockdivide(0);
		m_bus_init();
		m_disableJTAG();
		sei();
		setup();
		
		//Enable the IMU
		m_imu_init(accel_scale, gyro_scale);
		 
    while(1)
    {
        read();
        //Acceleration in Y centered at 0
        checkPoint = (int)(data[1] + 650);
        control();
    }
    return 0;
}
