#ifndef SDCARD_H_
#define SDCARD_H_
#include "serial.h"
#include "ff.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern "C" void TestSDWrite(Serial *serial, int lines,int doFlush, int quiet);
void InitFSHardware(void);
int InitFS();
int UnmountFS();
int OpenNextLogFile(FIL *f);

#ifdef __cplusplus
}
#endif


#endif /*SDCARD_H_*/
