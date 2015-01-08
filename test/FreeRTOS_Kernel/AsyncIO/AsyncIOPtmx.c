/*
 * AsyncIOPtmx.c
 *
 *  Created on: January 7, 2015
 *      Author: JS Stoezel
 */

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "AsyncIOSerial.h"
/*---------------------------------------------------------------------------*/

/* See 'man termios' for more details on configuring the serial port. */

long lAsyncIOPtmxOpen( const char *pcDevice, int *piDeviceDescriptor )
{
   int               iSerialDevice = 0;
   struct termios    orig_termios;

   iSerialDevice = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (iSerialDevice < 0)
	{
		printf("failed to open /dev/ptmx\r\n");
		return pdFALSE;
	}

	grantpt(iSerialDevice);
	unlockpt(iSerialDevice);
   
   // Disable echo on the terminal, when sending data
   if (tcgetattr (iSerialDevice, &orig_termios) < 0) {
      printf("ERROR getting current terminal's attributes\r\n");
      return pdFALSE;
   }
   orig_termios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
   orig_termios.c_oflag &= ~(ONLCR);
   if (tcsetattr (iSerialDevice, TCSANOW, &orig_termios) < 0) {
      printf ("ERROR setting current terminal's attributes\r\n");
      return pdFALSE;
   }

	printf("RaceCapture/Pro simulator %s interface on: %s local fd %d\n", pcDevice, ptsname(iSerialDevice), iSerialDevice);
	
	/* Pass out the device descriptor for subsequent calls to AsyncIORegisterCallback() */
	*piDeviceDescriptor = iSerialDevice;

	return pdTRUE;
}
 /*---------------------------------------------------------------------------*/

/* Define a callback function which is called when data is available. */
void vAsyncPtmxIODataAvailableISR( int iFileDescriptor, void *pContext )
{
	portBASE_TYPE 	xHigherPriorityTaskWoken = pdFALSE;
	portBASE_TYPE 	doEet = pdFALSE;
	ssize_t 		iReadResult = -1;
	unsigned char 	ucRx		= 0;
	unsigned int 	i 			= 0;
   
	do
	{
		/* This handler only processes a single byte/character at a time. */
		iReadResult = read( iFileDescriptor, &ucRx, 1 );
		if ( 1 == iReadResult )
		{
			if ( NULL != pContext )
			{
				/* Send the received byte to the queue. */
				if ( pdTRUE == xQueueSendFromISR( (xQueueHandle)pContext, &ucRx, &xHigherPriorityTaskWoken ) )
				{
				   if(xHigherPriorityTaskWoken)
				   {
					  doEet = 1;
				   }	
				}
				else
				{
					printf("failed to send on queue 0x08%X\r\n", pContext);
				}
			}
			else
			{
				printf("pContext is null\r\n");
			}
		}
	} while(1 == iReadResult);
	
	portEND_SWITCHING_ISR( doEet );
}
/*---------------------------------------------------------------------------*/
