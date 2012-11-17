/**
 * LED CPU Monitor
 * 
 * USB Request Definitions
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

 
#ifndef _USB_REQUESTS_H_
#define _USB_REQUESTS_H_

/* Request to echo a command to check connection.
 * Sends back wValue and wIndex
 */
#define	RQ_ECHO          0

/*
 * Request for the current status (LED values, current mixing type, current mixing value, etc)
 */
#define	RQ_STATUS        1

/*
 * Request from the host to update the device on the cpu usage. An integer will be sent from the host between 1 and 100.
 */
#define	RQ_SET_CPU_USAGE 2

#endif //_USB_REQUESTS_H_
