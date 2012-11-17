/**
 * LED CPU Status indicator
 * 
 * Firmware driver
 * 
 * Kevin Cuzner
 */

/***********************************************************************
 * This file is part of the USB LED CPU Monitor.
 * 
 * The USB LED CPU Monitor is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * The USB LED CPU Monitor is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with The USB LED CPU Monitor. If not, see
 * <http://www.gnu.org/licenses/>.
 **********************************************************************/


#define F_CPU 16000000

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "mixer.h"

#define	LED_PORT	PORTB
#define	LED_DDR 	DDRB
#define	LED_R_MASK	0x01
#define	LED_G_MASK	0x02
#define	LED_B_MASK	0x04

int main(void)
{
	//wdt_disable();
	
	unsigned char i;
	//EICRA |= 0x01;
    //EIMSK |= 0x01;
	usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        _delay_ms(1);
    }
    usbDeviceConnect();
    //DDRD = 0x00;
    //sei();
    
    mixInit();    
    
    //set up timer
    TCCR0B |= 0x05;
    TIMSK0 |= 0x01;
    
    sei();
	
	//LED_DDR |= LED_R_MASK | LED_G_MASK | LED_B_MASK; //set the pins as outputs
	//LED_PORT |= LED_R_MASK | LED_G_MASK | LED_B_MASK; //turn them all off
	//DDRB = 0xFF;
	while(1)
	{
		usbPoll();
		
		doMixMain();
		
		wdt_reset();
	}

	return 0;
}

ISR(TIMER0_OVF_vect)
{
	doMixInterrupt();
}

