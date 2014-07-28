#include "sdcard.h"

bool mounted = false;


void TestSDWrite(Serial *serial, int lines,int doFlush, int quiet, int delay){

}

void InitFSHardware(void){

}

int InitFS(){
   mounted = true;
	return 0;
}

int UnmountFS(){
   mounted = false;
	return 0;
}

int OpenNextLogFile(FIL *f){
	return 0;
}

bool isMounted() {
   return mounted;
}
