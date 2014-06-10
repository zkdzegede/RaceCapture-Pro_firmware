#ifndef __DATA_LOGGER_TASK_H__
#define __DATA_LOGGER_TASK_H__
#include "loggerConfig.h"
#include "FreeRTOS.h"
#include "ff.h"
#include "sampleRecord.h"


void startFileWriterTask( int priority );
void fileWriterTask(void *params);
portBASE_TYPE queue_logfile_record(LoggerMessage * sr);

#endif /* __DATA_LOGGER_TASK_H__ */
