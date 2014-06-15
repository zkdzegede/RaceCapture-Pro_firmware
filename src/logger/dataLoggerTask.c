/*
 * dataLoggerTask.c
 *
 * Responsible for writing RCP Log data to a file.
 *
 * Author: stieg
 */
#include "dataLoggerTask.h"
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

/*
 * Maximum size of the string to be written to the log file.  This is usually the header and
 * last I checked it wasn't more than 273 characters.  300 should be safe for now.
 */
#define MAX_LOG_STR_SIZE 300

struct file_data logFile = { .pfx = "rc_", .sfx = ".log", };

static struct {
    char *pos;
    char buff[MAX_LOG_STR_SIZE];
} strBuilder;

static void reset_string_builder() {
    strBuilder.pos = strBuilder.buff;
    *strBuilder.pos = '\0';
}

static void append_to_builder(const char *str) {
    if (str == NULL) return;

    while (*str) {
        *strBuilder.pos = *str;
        ++strBuilder.pos;
        ++str;
    }

    *strBuilder.pos = '\0';
}

static int write_builder_to_log() {
    int rc = append_to_file(&logFile, strBuilder.buff);
    reset_string_builder();
    return rc;
}

static xQueueHandle g_sampleRecordQueue = NULL;
#define FILE_WRITER_STACK_SIZE  				200
#define SAMPLE_RECORD_QUEUE_SIZE				10

//wait time for sample queue. can be portMAX_DELAY to wait forever, or zero to not wait at all
#define SAMPLE_QUEUE_WAIT_TIME					0

portBASE_TYPE queue_logfile_record(LoggerMessage * msg) {
    if (NULL != g_sampleRecordQueue) {
        return xQueueSend(g_sampleRecordQueue, &msg, SAMPLE_QUEUE_WAIT_TIME);
    } else {
        return errQUEUE_EMPTY;
    }
}

static void write_quoted_string(const char *s) {
    append_to_builder("\"");
    append_to_builder(s);
    append_to_builder("\"");
}

static void write_int(int num) {
    char buf[10];
    modp_itoa10(num, buf);
    append_to_builder(buf);
}

static void write_float(float num, int precision) {
    char buf[20];
    modp_ftoa(num, buf, precision);
    append_to_builder(buf);
}

static int write_headers(ChannelSample *channelSamples, size_t sampleCount) {
    int headerCount = 0;
    ChannelSample *sample = channelSamples;
    for (size_t i = 0; i < sampleCount; ++i, ++sample) {
        if (SAMPLE_DISABLED != sample->sampleRate) {
            if (headerCount++ > 0) append_to_builder(",");
            const Channel *field = get_channel(sample->channelId);
            write_quoted_string(field->label);
            append_to_builder("|");
            write_quoted_string(field->units);
            append_to_builder("|");
            write_int(decodeSampleRate(sample->sampleRate));
        }
    }
    append_to_builder("\n");
    return write_builder_to_log();
}

static int write_channel_samples(ChannelSample * channelSamples,
        size_t channelCount) {
    if (NULL != channelSamples) {
        pr_debug("null sample record\r\n");
        return 0;
    }

    int fieldCount = 0;
    for (size_t i = 0; i < channelCount; i++) {
        ChannelSample *sample = (channelSamples + i);

        if (fieldCount++ > 0) append_to_builder(",");
        if (NIL_SAMPLE == sample->intValue) continue;

        int precision = get_channel(sample->channelId)->precision;
        if (precision > 0) {
            write_float(sample->floatValue, precision);
        } else {
            write_int(sample->intValue);
        }
    }
    append_to_builder("\n");
    return write_builder_to_log();
}

void logWriterTask(void *params) {
    LoggerMessage *msg = NULL;

    while (1) {
        //wait for the next sample record
        xQueueReceive(g_sampleRecordQueue, &(msg), portMAX_DELAY);

        switch (msg->messageType) {
            case LOGGER_MSG_START_LOG:
                // This implicitly starts a new log file.
                write_headers(msg->channelSamples, msg->sampleCount);
                break;
            case LOGGER_MSG_END_LOG:
                close_file(&logFile);
                break;
            case LOGGER_MSG_SAMPLE:
                write_channel_samples(msg->channelSamples, msg->sampleCount);
                break;
        }
    }
}

void startFileWriterTask(int priority) {

    g_sampleRecordQueue = xQueueCreate(SAMPLE_RECORD_QUEUE_SIZE,
            sizeof(ChannelSample *));
    if (NULL == g_sampleRecordQueue) {
        pr_error("Could not create sample record queue!");
        return;
    }

    xTaskCreate( logWriterTask,(signed portCHAR * ) "log writer", FILE_WRITER_STACK_SIZE,
            NULL, priority, NULL);
}
