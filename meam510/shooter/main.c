/***** MEAM510 GOLF - Alfredo Muniz & Sade Oba *****/
#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_usb.h"

int IR[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int alignment = 32000;
int initilize1 = 0;
int initilize2 = 0;
int initilize3 = 0;
int usbInit = 0;
int alignReady = 1;
int holdReady = 0;
int fireReady = 0;
float magic = 0.764; //Found Experimentally for Cap Charging to Dist

//Whenever timer matches duration
ISR(TIMER3_COMPA_vect)
{	
	//Stop Charging the Cap after time finished
	clear(TCCR1A, COM1B1);
	clear(TCCR1A, COM1B0);
	
	fireReady = 1;
}

/** Da Super Wasteful ADC Code.Y U NO flip more than 1 bit per line? **/
ISR(ADC_vect)
{
	//Read the Result
	z = z+1;
	switch(z)
	{
		case 1:
			IR[0] = ADC;
			set(PORTB, 0);
			clear(PORTB, 1);
			clear(PORTB, 2);
			clear(PORTB, 3);
			break;
		case 2:
			IR[1] = ADC;
			clear(PORTB, 0);
			set(PORTB, 1);
			clear(PORTB, 2);
			clear(PORTB, 3);
			break;
		case 3:
			IR[2] = ADC;
			set(PORTB, 0);
			set(PORTB, 1);
			clear(PORTB, 2);
			clear(PORTB, 3);
			break;
		case 4:
			IR[3] = ADC;
			clear(PORTB, 0);
			clear(PORTB, 1);
			set(PORTB, 2);
			clear(PORTB, 3);	
			break;
		case 5:
			IR[4] = ADC;
			set(PORTB, 0);
			clear(PORTB, 1);
			set(PORTB, 2);
			clear(PORTB, 3);
			break;
		case 6:
			IR[5] = ADC;
			clear(PORTB, 0);
			set(PORTB, 1);
			set(PORTB, 2);
			clear(PORTB, 3);
			break;
		case 7:
			IR[6] = ADC;
			set(PORTB, 0);
			set(PORTB, 1);
			set(PORTB, 2);
			clear(PORTB, 3);
			break;
		case 8:
			IR[7] = ADC;
			clear(PORTB, 0);
			clear(PORTB, 1);
			clear(PORTB, 2);
			set(PORTB, 3);
			break;
		case 9:
			IR[8] = ADC;
			set(PORTB, 0);
			clear(PORTB, 1);
			clear(PORTB, 2);
			set(PORTB, 3);
			break;
		case 10:
			IR[9] = ADC;
			clear(PORTB, 0);
			set(PORTB, 1);
			clear(PORTB, 2);
			set(PORTB, 3);
			break;
		case 11:
			IR[10] = ADC;
			set(PORTB, 0);
			set(PORTB, 1);
			clear(PORTB, 2);
			set(PORTB, 3);
			break;
		case 12:
			IR[11] = ADC;
			clear(PORTB, 0);
			clear(PORTB, 1);
			set(PORTB, 2);
			set(PORTB, 3);
			break;
		case 13:
			IR[12] = ADC;
			set(PORTB, 0);
			clear(PORTB, 1);
			set(PORTB, 2);
			set(PORTB, 3);
			break;
		case 14:
			IR[13] = ADC;
			clear(PORTB, 0);
			set(PORTB, 1);
			set(PORTB, 2);
			set(PORTB, 3);
			break;
		case 15:
			IR[14] = ADC;
			set(PORTB, 0);
			set(PORTB, 1);
			set(PORTB, 2);
			set(PORTB, 3);
			break;
		case 16:
			IR[15] = ADC;
			clear(PORTB, 0);
			clear(PORTB, 1);
			clear(PORTB, 2);
			clear(PORTB, 3);
			z = 0;
			break;
		default:
			m_red(ON); // something is wrong
	}
}

//Setting up the poop ADC like in lab 1
int setupADC(void)
{
	if(initialize1 == 0)
	{
		//Setup MUX Outputs B0 B1 B2 B3
		//Set all B0-B3 as outputs
		set(DDRB, 0);
		set(DDRB, 1);
		set(DDRB, 2);
		set(DDRB, 3);
		//Bring to Zero all B0-B3
		clear(PORTB, 0);
		clear(PORTB, 1);
		clear(PORTB, 2);
		clear(PORTB, 3);
		
		//Set Voltage Reference
		clear(ADMUX, REFS1);
		set(ADMUX, REFS0);
		
		//ADC Prescalar
		set(ADCSRA, ADPS2);
		set(ADCSRA, ADPS1);
		set(ADCSRA, ADPS0);
		
		//Disable Digital Inputs
		set(DIDR0, ADC0D);
				
		//Interrupts
		set(ADCSRA, ADIE);
		sei();
		
		//Triggering
		set(ADCSRA, ADATE);
		
		//Single-Ended Channel Selection
		clear(ADCSRB, MUX5);
		clear(ADCSRB, MUX2);
		clear(ADCSRB, MUX1);
		clear(ADCSRB, MUX0);
		
		//Starting Conversion
		set(ADCSRA, ADEN);
		set(ADCSRA, ADSC);
		
		initialize1 = 1;
	}
	return 1;
}

//Control for finding the beacon and firing
int align(void)
{	
	if(alignReady)
	{
		//Start Motor
		set(TCCR1A, COM1C1);
		set(TCCR1A, COM1C0);
		
		//Try to align itself multiple times
		for(i = 0; i > alignment; i++)
		{
			//Use only the front 2 IRs
			if(IR[7] > IR[8])
			{
				set(PORTD, 5);
			}
			else
			{
				clear(PORTD, 5);
			}
		}
		//Duration to charge the cap
		duration = magic*(IR[7]+IR[8])/2;
		
		//Stop Motor
		clear(TCCR1A, COM1C1);
		clear(TCCR1A, COM1C0);
		
		holdReady = 1;
		alignReady = 0;
		fireReady = 0;
	}
	
	return 0;
}

//Turn on the motor and charge the cap
int setupPWM(void)
{	
	if(initialize2 == 0)
	{
		sei();
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
		//DO NOT START THE CAP CHARGING YET!
		clear(TCCR1A, COM1B1);
		clear(TCCR1A, COM1B0);
		//Stop the motor
		clear(TCCR1A, COM1C1);
		clear(TCCR1A, COM1C0);
		
		initialize2 = 1;
	}	
	return 1;
}

int hold()
{
	if(initialize3 == 0)
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
		
		initialize3 = 1; 
	}
	
	if(holdReady)
	{
		if(duration < 0)
		{
			duration = 30;
		}
		
		//Equation from using the oscope and pin C6
		OCR3A = (156*duration);
		
		//Start Charging Cap
		set(TCCR1A, COM1C1);
		set(TCCR1A, COM1C0);
		
		holdReady = 0;
		//It'll be stuck until interrupt with timer triggers the fire
	}
	return 0;
}

int fire(void)
{
	if(fireReady)
	{
		set(PORTD, 6)
		m_wait(20);
		clear(PORTD, 6);
		alignReady = 1;
		fireReady = 0;
	}
	
	return 0;
}

//Debugs the ADC just in case something weird happens
int debug()
{
	if(usbInit == 0)
	{
		m_usb_init();
		usbInit = 1;
	}
	
	if(m_usb_isconnected())
	{	
		m_usb_tx_string("\r\n Zero:  ");
		m_usb_tx_int(IR[0]);
		m_usb_tx_string("\r\n One:  ");
		m_usb_tx_int(IR[1]);
		m_usb_tx_string("\r\n Two:  ");
		m_usb_tx_int(IR[2]);
		m_usb_tx_string("\r\n Three:  ");
		m_usb_tx_int(IR[3]);
		m_usb_tx_string("\r\n Four:  ");
		m_usb_tx_int(IR[4]);
		m_usb_tx_string("\r\n Five:  ");
		m_usb_tx_int(IR[5]);
		m_usb_tx_string("\r\n Six:  ");
		m_usb_tx_int(IR[6]);
		m_usb_tx_string("\r\n Seven:  ");
		m_usb_tx_int(IR[7]);
		m_usb_tx_string("\r\n Eight:  ");
		m_usb_tx_int(IR[8]);
		m_usb_tx_string("\r\n Nine:  ");
		m_usb_tx_int(IR[9]);
		m_usb_tx_string("\r\n Ten:  ");
		m_usb_tx_int(IR[10]);
		m_usb_tx_string("\r\n Eleven:  ");
		m_usb_tx_int(IR[11]);
		m_usb_tx_string("\r\n Twelve:  ");
		m_usb_tx_int(IR[12]);
		m_usb_tx_string("\r\n Thirteen:  ");
		m_usb_tx_int(IR[13]);
		m_usb_tx_string("\r\n Fourteen:  ");
		m_usb_tx_int(IR[14]);
		m_usb_tx_string("\r\n Fifteen:  ");
		m_usb_tx_int(IR[15]);
	}
	
	return 0;
}

int main(void)
{
		DDRE |= 1<<6;
		PORTE &= !(1<<6);
		m_clockdivide(0);
		m_bus_init();
		m_disableJTAG();
		sei();
		setupADC(); //sets up adc for the IRs
		setupPWM(); //PWM for boost converter and for motors
		hold();
			
        while(1)
        {
			align();
			hold();
			fire(); //The Solenoid that is. NOT the robot!
		}
}  
