/**
 * LED CPU Monitor
 * 
 * LED Mixer with multiple modes
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


#ifndef _MIXER_H_
#define _MIXER_H_

#define	LED_PORT	PORTB
#define	LED_DDR 	DDRB
#define	LED_R_MASK	0x01
#define	LED_G_MASK	0x02
#define	LED_B_MASK	0x04

#define MIX_INTERRUPT_ENABLE_REG	TIMSK0
#define	MIX_INTERRUPT_ENABLE_MASK	0x01

//when the mixer has not had its cpu usage updated in a while, it goes into idling mode. The idling speed is how slow (bigger = slower) the colors will rotate around the circle of colors
#define MIX_IDLING_SPEED         8

#define MIX_MODE_SOLID_DELTA     3 //the delta value to use when mixing between solid colors

/* Called by external functions to set the cpu usage */
void setCPUUsage(unsigned char usage);
/* Should be called when the program starts */
void mixInit(void);
/* Should be placed in the tmr0 interrupt */
void doMixInterrupt(void);
/* Should be placed in the main loop */
void doMixMain(void);

#endif //_MIXER_H_
