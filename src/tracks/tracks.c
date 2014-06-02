#include "tracks.h"
#include "mod_string.h"
#include "printk.h"
#include "memory.h"
#include "mem_mang.h"

#ifndef RCP_TESTING
#include "memory.h"
static const struct Track[] g_tracks __attribute__ ((aligned (FLASH_MEMORY_PAGE_SIZE))) __attribute__((section(".tracks\n\t#")));
#else
static const struct Track[] g_tracks = DEFAULT_TRACKS;
#endif

