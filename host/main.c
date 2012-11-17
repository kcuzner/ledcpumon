/**
 * LED CPU Status indicator
 * 
 * Host side software
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


#include <stdio.h>

#define DAEMON_FLAG    1

#include <unistd.h> //this is supported by my kernel (3.0.0-17), but if it isn't google daemonizing a process to replace it
#include <usb.h>
#include "opendevice.h"
#include "../include/usbconfig.h"
#include "../include/usbrequests.h"

int main(int argc, char* argv[])
{
	usb_dev_handle      *handle = NULL;
	const unsigned char rawVid[2] = {USB_CFG_VENDOR_ID}, rawPid[2] = {USB_CFG_DEVICE_ID};
	char                vendor[] = {USB_CFG_VENDOR_NAME, 0}, product[] = {USB_CFG_DEVICE_NAME, 0};
	char                buffer[4];
	int                 vid, pid;
	
	int flags = 0;
	int opt;
	while((opt = getopt(argc, argv, "d")) != -1)
	{
		switch(opt)
		{
			case 'd':
				//daemonize flag
				flags |= DAEMON_FLAG;
				break;
			default:
				//some weird one
				fprintf(stderr, "Usage: %s [-d]\n\t-d\tRun as daemon\n", argv[0]);
				exit(EXIT_FAILURE); //leave
				break;
		}
	}
	
	if (flags & DAEMON_FLAG)
	{
		//we get to daemonize!
		if (daemon(0,0) < 0) {
			//error trying to daemonize
			exit(EXIT_FAILURE);
		}
	}
	
	usb_init();
	/* compute VID/PID from usbconfig.h so that there is a central source of information */
    vid = rawVid[1] * 256 + rawVid[0];
    pid = rawPid[1] * 256 + rawPid[0];
    
    //cpu usage variables
    int totalJiffiesLast = -1;
    int workJiffiesLast = -1;
    int totalJiffiesCurrent, workJiffiesCurrent;
    while(1)
    {
		/**
		 * Each iteration we will connect to the device and also poll the computers stat file
		 */
		
		
		/* The following function is in opendevice.c: */
		int result = usbOpenDevice(&handle, vid, vendor, pid, product, NULL, NULL, NULL);
		if(result != 0){
			fprintf(stderr, "Could not find USB device \"%s\" with vid=0x%x pid=0x%x (error: %i)\n", product, vid, pid, result);
			sleep(1);
			continue; //try again
			//exit(1);
		}
		
		/* Read cpu usage */
		FILE* fp = fopen("/proc/stat", "r");
		if (fp == NULL)
		{
			printf("Failed to open /proc/stat...\n");
			continue;
		}
		fscanf(fp, "%*s");
		int temp;
		totalJiffiesCurrent = 0;
		int user, nice, sys;
		if (fscanf(fp, "%d %d %d", &user, &nice, &sys) == 0)
		{
			printf("Problem with format of /proc/stat...\n");
			continue;
		}
		totalJiffiesCurrent = workJiffiesCurrent = user + nice + sys;
		//read out the rest of the numbers in the first line
		while(fscanf(fp, "%d", &temp) == 1)
		{
			totalJiffiesCurrent += temp;
		}
		fclose(fp);
		
		/* Compute CPU Usage */
		float percentUsage = 0.0f;
		if (totalJiffiesLast != -1)
		{
			//compare the jiffies
			int workDone = workJiffiesCurrent - workJiffiesLast;
			int totalDone = totalJiffiesCurrent - totalJiffiesLast;
			percentUsage = (float)workDone / (float)totalDone * 100.0f;
			//printf("CPU: %f percent\n", percentUsage);
			
			/* Tell the device our cpu usage */
			usb_control_msg( handle, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, RQ_SET_CPU_USAGE, (int)percentUsage, 0, buffer, 0, 1000 );
		}
		
		//put our jiffies into the last jiffies to save for the next round
		totalJiffiesLast = totalJiffiesCurrent;
		workJiffiesLast = workJiffiesCurrent;
		
		//close the usb handle
		usb_close(handle);
		
		usleep(500000);
	}
	
	return 0;
}
