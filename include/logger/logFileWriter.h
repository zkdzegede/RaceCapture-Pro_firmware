#ifndef _FILEWRITER_H_
#define _FILEWRITER_H_

#include "ff.h"
#include "flags.h"
#include "FreeRTOS.h"

#define MAX_FILENAME_LEN					    32

enum {
    FS_OPEN = FLAG_BIT(0),
};

struct file_data {
    FIL file_handle;
    int file_status;
    portTickType lastFlushTick;
    char *pfx;
    char *sfx;
    char fPath[MAX_FILENAME_LEN];
};

int close_file(struct file_data *fd);
int open_file(struct file_data *fd);
int append_to_file(struct file_data *fd, const char *data);

#endif /* _FILEWRITER_H_ */
