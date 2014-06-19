/*
 * fileWriter.c
 *
 *  Created on: Feb 29, 2012
 *      Author: brent
 */
#include "logFileWriter.h"
#include "task.h"
#include "semphr.h"
#include "modp_numtoa.h"
#include "sdcard.h"
#include "sampleRecord.h"
#include "loggerHardware.h"
#include "taskUtil.h"
#include "mod_string.h"
#include "printk.h"
#include "spi.h"
#include "channelMeta.h"
#include "mem_mang.h"
#include "LED.h"

enum {
    FS_UNMOUNTED, FS_MOUNTED
} mount_status;

#define MAX_FILE_INDEX                          99999
#define MAX_WRITE_ATTEMPTS                      2
#define MAX_SYNC_INTERVAL_MS                    3000
#define ERROR_SLEEP_DELAY_MS					1000
#define MAX_SIMULTANIOUS_FILES					3
#define WRITE_SUCCESS  0
#define WRITE_FAIL     EOF
#define FILE_WRITE(F,S) f_puts(S, F)

/* Hacky way I am tracking the structs.  Should use lists like in Linux Kernel */
static struct file_data *logFileData[MAX_SIMULTANIOUS_FILES] = { 0 };

static void set_fs_error() {
    LED_enable(3);
}

static void clear_fs_error() {
    LED_disable(3);
}

static int addFileDataStructToListIfNotPresent(struct file_data *ptr) {
    int i;

    for (i = 0; i < MAX_SIMULTANIOUS_FILES; ++i) {
        if (logFileData[i] == ptr) break;
        if (logFileData[i] == NULL) {
            logFileData[i] = ptr;
            break;
        }
    }

    return i >= MAX_SIMULTANIOUS_FILES ? -1 : i;
}

static int open_new_file(struct file_data *fd, char *filename) {
    pr_info("Creating new file ");
    pr_info(filename);
    pr_info("\r\n");

    const int rc = f_open(&(fd->file_handle), filename,
    FA_WRITE | FA_CREATE_NEW);

    if (FR_OK == rc) {
        fd->file_status |= FS_OPEN;
        fd->lastFlushTick = xTaskGetTickCount();
        addFileDataStructToListIfNotPresent(fd);
    } else {
        pr_warning("Failed to create file ");
        pr_warning(filename);
        pr_warning("\r\n");
    }

    return rc;
}

static int mount_fs() {
    pr_info("Mounting Filesystem\r\n");
    const int rc = InitFS();

    if (FR_OK != rc) {
        pr_error("FS mount error\r\n");
    } else {
    	mount_status = FS_MOUNTED;
    }

    return rc;
}


static int unmount_fs() {
    pr_info("Un-mounting Filesystem\r\n");
    const int rc = UnmountFS();
    mount_status = FS_UNMOUNTED; // If this fails, treat us as unmounted anyways.

    if (FR_OK != rc) {
        pr_error("FS unmount error\r\n");
    }

    return rc;
}


static int sync_file(struct file_data *fd) {
	if (! (fd->file_status & FS_OPEN)) {
		return -1;
	}

    const int res = f_sync(&(fd->file_handle));

    if (FR_OK != res) {
        pr_debug_int(res);
        pr_debug(" = flush error\r\n");
    } else {
        fd->lastFlushTick = xTaskGetTickCount();
    }

    return res;
}

static int create_filename(char *buf, size_t size, const char *pfx, const char *sfx,
        int num) {
    char numBuf[12];
    modp_itoa10(num, numBuf);

    memset(buf, 0, size);

    size_t len = strlen(pfx) + strlen(sfx) + sizeof(numBuf);
    if (len > size) {
        pr_warning("filename larger than buffer size. ");
        pr_warning_int(len);
        pr_warning(" > ");
        pr_warning_int(size);
        pr_warning(" \r\n");
        return -1;
    }

    if (pfx) strcat(buf, pfx);
    strcat(buf, numBuf);
    if (sfx) strcat(buf, sfx);
    return 0;
}

static int open_new_writeable_file(struct file_data *fd) {
    int i, rc;
    char buf[MAX_FILENAME_LEN] = { 0 };

    for (i = 0; i < MAX_FILE_INDEX; i++) {
        int result = create_filename(buf, sizeof(buf), fd->pfx, fd->sfx, i);
        if (result != 0) {
            pr_error("Error making fileName.");
            return result;
        }

        rc = open_new_file(fd, buf);
        if (FR_OK == rc) {
            pr_info("Successfully opened file \"");
            pr_info(buf);
            pr_info("\".  Associated with file_data struct ");
            pr_info_int((int )fd);
            pr_info("\r\n");

            strcpy(fd->fPath, buf);
            break;
        }
    }

    if (i >= MAX_FILE_INDEX) return -2;
    return rc;
}

// Closes all files, remounts, and opens files if they were open before.
static void try_fs_recovery() {
	int i;
	for (i = 0; i < MAX_SIMULTANIOUS_FILES; ++i) {
		struct file_data *fd = logFileData[i];
		if (fd == NULL) continue;
		f_close(&(fd->file_handle));
	}

	unmount_fs();
	int status = mount_fs();

	for (i = 0; i < MAX_SIMULTANIOUS_FILES; ++i) {
		struct file_data *fd = logFileData[i];
		if (fd == NULL) continue;

		if (status == FR_OK) {
			if (fd->file_status & FS_OPEN) open_file(fd);
		} else {
			close_file(fd);
		}
	}
}

int open_file(struct file_data *fd) {
    if (fd->fPath == NULL) {
    	pr_warning("File Path is null.  Can't open.\r\n");
    	return -1;
    }

    pr_info("Opening ");
    pr_info(fd->fPath);
    pr_info("\r\n");

    const int rc = f_open(&(fd->file_handle), fd->fPath, FA_WRITE);

    if (FR_OK == rc) {
        fd->file_status |= FS_OPEN;
        fd->lastFlushTick = xTaskGetTickCount();
        addFileDataStructToListIfNotPresent(fd);
    } else {
        pr_warning("Failed to open file ");
        pr_warning(fd->fPath);
        pr_warning("\r\n");
    }

    return rc;
}

int close_file(struct file_data *fd) {
    pr_info("Closing \r\n");

    const int rc = f_close(&(fd->file_handle));
    fd->file_status &= ~FS_OPEN; // Closed, even if failed.

    if (FR_OK != rc) {
        pr_warning("Failed to close file \r\n");
    }

    return rc;
}

int append_to_file(struct file_data *fd, const char *data) {
    int status = FR_OK;
    lock_spi();

    if (mount_status != FS_MOUNTED) {
        status = mount_fs();
        if (FR_OK != status) goto out;
    }

    if (!(fd->file_status & FS_OPEN)) {
        status = open_new_writeable_file(fd);
        if (FR_OK != status) goto out;
    }

    int try = 1;
    while (1) {
        status = f_puts(data, &(fd->file_handle));

        if (FR_OK == status) break;

        if (try >= MAX_WRITE_ATTEMPTS) {
            pr_warning("Failed to write data with err code ");
            pr_warning_int(status);
            pr_warning("\r\n");
            goto out;
        }

        try_fs_recovery();
        ++try;
    }

    if (isTimeoutMs(fd->lastFlushTick, MAX_SYNC_INTERVAL_MS)) {
        status = sync_file(fd);
        if (FR_OK != status) goto out;
    }

    out:
    unlock_spi();
    status == FR_OK ? clear_fs_error() : set_fs_error();
    return status;
}

