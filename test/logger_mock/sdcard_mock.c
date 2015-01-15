#include <stdio.h>
#include "sdcard.h"

void TestSDWrite(Serial *serial, int lines,int doFlush, int quiet)
{
   printf("TestSDWrite %d\r\n", serial);
}

void InitFSHardware(void){

}

int InitFS(){
	return 0;
}

int UnmountFS(){
	return 0;
}

int OpenNextLogFile(FIL *f){
	return 0;
}
