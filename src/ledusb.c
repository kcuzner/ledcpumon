/**
 * USB portion of firmware (outside the v-usb driver)
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

#include <avr/io.h>

#include <avr/pgmspace.h>
#include "usbdrv.h"
#include "usbrequests.h"
#include "mixer.h"

extern unsigned char r_value;
extern unsigned char g_value;
extern unsigned char b_value;

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
	usbRequest_t    *rq = (void *)data;
	static uchar    dataBuffer[4];  /* buffer must stay valid when usbFunctionSetup returns */
	

	if(rq->bRequest == RQ_ECHO) /* echo -- used for reliability tests */
	{
		dataBuffer[0] = rq->wValue.bytes[0];
		dataBuffer[1] = rq->wValue.bytes[1];
		dataBuffer[2] = rq->wIndex.bytes[0];
		dataBuffer[3] = rq->wIndex.bytes[1];
		usbMsgPtr = dataBuffer;         /* tell the driver which data to return */
		return 4;
	}
	else if (rq->bRequest == RQ_SET_CPU_USAGE)
	{
		setCPUUsage(rq->wValue.bytes[0]);
		return 0;
	}
	
    return 0;   /* default for not implemented requests: return no data back to host */
}
