#include "ff.h"

FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode){return FR_OK;};				/* Open or create a file */
FRESULT f_close (FIL* fp){return FR_OK;};											/* Close an open file object */
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br){return FR_OK;};			/* Read data from a file */
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw){return FR_OK;};	/* Write data to a file */
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT),
		UINT btf, UINT* bf){return FR_OK;};										/* Forward data to the stream */
FRESULT f_lseek (FIL* fp, DWORD ofs){return FR_OK;};								/* Move file pointer of a file object */
FRESULT f_truncate (FIL* fp){return FR_OK;};										/* Truncate file */
FRESULT f_sync (FIL* fp){return FR_OK;};											/* Flush cached data of a writing file */
FRESULT f_opendir (DIR* dp, const TCHAR* path){return FR_OK;};						/* Open a directory */
FRESULT f_closedir (DIR* dp){return FR_OK;};										/* Close an open directory */
FRESULT f_readdir (DIR* dp, FILINFO* fno){return FR_OK;};							/* Read a directory item */
FRESULT f_mkdir (const TCHAR* path){return FR_OK;};								/* Create a sub directory */
FRESULT f_unlink (const TCHAR* path){return FR_OK;};								/* Delete an existing file or directory */
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new){return FR_OK;};	/* Rename/Move a file or directory */
FRESULT f_stat (const TCHAR* path, FILINFO* fno){return FR_OK;};					/* Get file status */
FRESULT f_chmod (const TCHAR* path, BYTE value, BYTE mask){return FR_OK;};			/* Change attribute of the file/dir */
FRESULT f_utime (const TCHAR* path, const FILINFO* fno){return FR_OK;};			/* Change times-tamp of the file/dir */
FRESULT f_chdir (const TCHAR* path){return FR_OK;};								/* Change current directory */
FRESULT f_chdrive (const TCHAR* path){return FR_OK;};								/* Change current drive */
FRESULT f_getcwd (TCHAR* buff, UINT len){return FR_OK;};							/* Get current directory */
FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs){return FR_OK;};	/* Get number of free clusters on the drive */
FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* sn){return FR_OK;};	/* Get volume label */
FRESULT f_setlabel (const TCHAR* label){return FR_OK;};							/* Set volume label */
FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt){return FR_OK;};			/* Mount/Unmount a logical drive */
FRESULT f_mkfs (const TCHAR* path, BYTE sfd, UINT au){return FR_OK;};				/* Create a file system on the volume */
FRESULT f_fdisk (BYTE pdrv, const DWORD szt[], void* work){return FR_OK;};			/* Divide a physical drive into some partitions */
int f_putc (TCHAR c, FIL* fp){return FR_OK;};										/* Put a character to the file */
int f_puts (const TCHAR* str, FIL* cp){return FR_OK;};								/* Put a string to the file */
int f_printf (FIL* fp, const TCHAR* str, ...){return FR_OK;};						/* Put a formatted string to the file */
TCHAR* f_gets (TCHAR* buff, int len, FIL* fp){return '\0';};						/* Get a string from the file */
