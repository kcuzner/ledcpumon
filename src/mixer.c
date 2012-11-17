/**
 * LED CPU Monitor
 * 
 * LED Mixer
 * 
 * Kevin Cuzner
 * 
 * 
 * Color map for cpu usage:
 * 0 = blue
 * 25 = cyan
 * 50 = green
 * 75 = yellow
 * 100 = red
 * 
 * Default color when device is powered on will be magenta and mixing mode will be fade to white.
 * 
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


#include <avr/io.h>
#include "usbrequests.h"
#include "mixer.h"

typedef struct
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} rgb_value;

volatile struct
{
	rgb_value wanted;
	rgb_value current;
} led_status;
volatile unsigned char cpuUsage;
volatile struct
{
	unsigned mode:4; //this holds the current mixing mode we are in (for future use)
	unsigned char status[4]; //this is used by each mode to store internal states (for future use)
	unsigned char timeSinceLastUpdate;
} mixing_mode;

/* Converts a cpu usage value into a rgb value. Values from 0 to 150 will be parsed so that we get a full color wheel with 25 states per section */
rgb_value usageToRGB(int usage)
{
	rgb_value ret;
	
	static char idlingCounter = 0;
	static char idlingValue = 0; //when the usage is invalid, this will replace the usage so that the colors rotate
	
	if (usage > 150)
	{
		//if the usage is wrong or is purposely not being set properly, begin to rotate around the colors in idle mode
		usage = idlingValue;
		idlingCounter++;
		
		if (idlingCounter >= MIX_IDLING_SPEED)
		{
			idlingCounter = 0;
			idlingValue++;
			
			if (idlingValue >= 150)
			{
				idlingValue = 0;
			}
		}
	}
	
	if (usage >= 0 && usage < 25)
	{
		//blue on full, green increases with usage
		ret.r = 0;
		ret.b = 255;
		ret.g = cpuUsage * 10;
	}
	else if (usage >= 25 && usage < 50)
	{
		//green on full, blue decreases with usage
		ret.r = 0;
		ret.g = 255;
		ret.b = 255 - ( (cpuUsage - 25) * 10 );
		
	}
	else if (usage >= 50 && usage < 75)
	{
		//green on full, red increases with usage
		ret.g = 255;
		ret.b = 0;
		ret.r = (usage - 50) * 10;
	}
	else if (usage >= 75 && usage < 100)
	{
		//red on full, green decreases with usage
		ret.r = 255;
		ret.b = 0;
		ret.g = 255 - ( (usage - 75) * 10 );
	}
	else if (usage >= 100 && usage < 125)
	{
		//red on full, blue increases with usage
		ret.r = 255;
		ret.g = 0;
		ret.b = (usage - 100) * 10;
	}
	else
	{
		//blue on full, red decreases with usage
		ret.b = 255;
		ret.g = 0;
		ret.r = 255 - ( (usage - 125) * 10 );
	}
	
	return ret;
}

/* Initializes the mixer */
void mixInit(void)
{
	setCPUUsage(255);
}

/* Sets the cpu usage */
void setCPUUsage(unsigned char usage)
{
	cpuUsage = usage;
	
	mixing_mode.timeSinceLastUpdate = 0;
	
	//disable our interrupt while we are messing with this
	MIX_INTERRUPT_ENABLE_REG &= ~MIX_INTERRUPT_ENABLE_MASK;
	//re-enable our interrupt
	MIX_INTERRUPT_ENABLE_REG |= MIX_INTERRUPT_ENABLE_MASK;
}

/* Should be called during the TMR0 overflow */
void doMixInterrupt(void)
{
	mixing_mode.timeSinceLastUpdate++;
	if (mixing_mode.timeSinceLastUpdate == 0)
	{
		setCPUUsage(255); //oh noes! no response
	}
	//turn cpu usage into a color
	led_status.wanted = usageToRGB(cpuUsage);
	
	//do color smoothing
	if (led_status.current.r > led_status.wanted.r)
	{
		led_status.current.r -= MIX_MODE_SOLID_DELTA;
	}
	else if (led_status.current.r < led_status.wanted.r)
	{
		led_status.current.r += MIX_MODE_SOLID_DELTA;
	}
	
	if (led_status.current.g > led_status.wanted.g)
	{
		led_status.current.g -= MIX_MODE_SOLID_DELTA;
	}
	else if (led_status.current.g < led_status.wanted.g)
	{
		led_status.current.g += MIX_MODE_SOLID_DELTA;
	}
	
	if (led_status.current.b > led_status.wanted.b)
	{
		led_status.current.b -= MIX_MODE_SOLID_DELTA;
	}
	else if (led_status.current.b < led_status.wanted.b)
	{
		led_status.current.b += MIX_MODE_SOLID_DELTA;
	}
}

/* Called during the main loop. This handles the pwm for the LEDs */
void doMixMain(void)
{
	static unsigned char i = 0;
	
	LED_DDR |= LED_R_MASK | LED_G_MASK | LED_B_MASK; //outputs
	
	if (i < led_status.current.r)
		LED_PORT |= LED_R_MASK; //turn on the red
	else
		LED_PORT &= ~LED_R_MASK; //turn off the red
	
	if (i < led_status.current.g)
		LED_PORT |= LED_G_MASK; //turn on the green
	else
		LED_PORT &= ~LED_G_MASK; //turn off the green
	
	if (i < led_status.current.b)
		LED_PORT |= LED_B_MASK; //turn on the blue
	else
		LED_PORT &= ~LED_B_MASK; //turn off the blue
	
	i++;
}
