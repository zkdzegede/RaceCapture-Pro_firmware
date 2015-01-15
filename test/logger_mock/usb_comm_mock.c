#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "usb_comm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mod_string.h"
#include "modp_numtoa.h"
#include "modp_atonum.h"
#include "memory.h"
#include "serial.h"
#include "messaging.h"
#include "AsyncIO.h"
#include "AsyncIOPtmx.h"
#include "queue.h"
#include "printk.h"

#define BUFFER_SIZE 1025

static char    lineBuffer[BUFFER_SIZE];
xQueueHandle 	xUsbCommRx	= 0;
int				usbCommFd	= -1;


#define mainUSB_COMM_STACK	( 1000 )

static int usb_comm_init(){
   int result = 0;
   /* Set-up the Serial Console Echo task */
	if ( pdTRUE == lAsyncIOPtmxOpen( "USB_COMM", &usbCommFd ) )
	{
		xUsbCommRx = xQueueCreate(BUFFER_SIZE,
			( unsigned portBASE_TYPE ) sizeof( signed portCHAR ));
		(void)lAsyncIORegisterCallback( usbCommFd, vAsyncPtmxIODataAvailableISR, xUsbCommRx );
		result = 1;
	}
	else
	{
		 pr_error("Failed to initialize USB COMM\r\n");
	}
	return result;
}

void usb_init_serial(Serial *serial){
	serial->init = &usb_init;
	serial->flush = &usb_flush;
	serial->get_c = &usb_getchar;
	serial->get_c_wait = &usb_getcharWait;
	serial->get_line = &usb_readLine;
	serial->get_line_wait = &usb_readLineWait;
	serial->put_c = &usb_putchar;
	serial->put_s = &usb_puts;
}

void startUSBCommTask(int priority){
	xTaskCreate( onUSBCommTask,( signed portCHAR * ) "OnUSBComm",
		     mainUSB_COMM_STACK, NULL, priority, NULL );
}

void onUSBCommTask(void *pvParameters) {
	usb_comm_init();
	
	Serial *serial = get_serial(SERIAL_USB);
	while (1) {
		process_msg(serial, lineBuffer, BUFFER_SIZE);
	}
}

void usb_init(unsigned int bits, unsigned int parity,
	      unsigned int stopBits, unsigned int baud){
	//null function - does not apply to USB CDC
}

void usb_flush(void)
{
	char c;
	while(usb_getcharWait(&c, 0));
}

int usb_getcharWait(char *c, size_t delay){
	if(0 != xUsbCommRx)
	{
	   return xQueueReceive( xUsbCommRx, c, delay ) == pdTRUE ? 1 : 0;
    }
    else
    {
		pr_error("Warning queue for USB COMM is not created yet");
		return 0;
	}
}

char usb_getchar(void){
	char c;
	usb_getcharWait(&c, portMAX_DELAY);
	return c;
}

int usb_readLine(char *s, int len)
{
	return usb_readLineWait(s,len,portMAX_DELAY);
}

int usb_readLineWait(char *s, int len, size_t delay)
{
	int count = 0;
	while(count < len - 1){
		char c = 0;
		if (!usb_getcharWait(&c, delay)) break;
		*s++ = c;
		count++;
		if (c == '\r')
		{
			*(s-1)='\n';
			break;
		}
	}
	*s = '\0';
	return count;
}

void usb_puts(const char *s){
	if(strlen(s) != write(usbCommFd, s, strlen(s)))
   {
      pr_error("\r\nFailed to write to USB COMM\r\n");
  }
}

void usb_putchar(char c){
	if(1 != write(usbCommFd, &c, 1))
      pr_error("\r\nFailed to write to USB COMM\r\n");
}
