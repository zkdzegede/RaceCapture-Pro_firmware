#include <3gpp.h>
#include "mod_string.h"
#include "printk.h"
#include "devices_common.h"

#define COMMAND_WAIT    600

static struct _3gppConnection _3gppConnections[MAX_3GPP_CONNECTIONS];

static int strstartswith(const char * __restrict string, const char * __restrict prefix)
{
    while(*prefix)
    {
        if(*prefix++ != *string++)
            return 0;
    }
    return 1;
}


static int _3gpp_read_wait(DeviceConfig *config, size_t delay)
{
    int c = config->serial->get_line_wait(config->buffer, config->length, delay);
    return c;
}

static void _3gpp_flush(DeviceConfig *config)
{
    config->buffer[0] = '\0';
    config->serial->flush();
}

void _3gpp_puts(DeviceConfig *config, const char *data)
{
    config->serial->put_s(data);
}

static int _3gpp_read_response(DeviceConfig *config, const char *rsp, size_t wait)
{
    _3gpp_read_wait(config, wait);
    pr_debug_str_msg("3gpp: cmd rsp: ", config->buffer);
    int res = strncmp(config->buffer, rsp, strlen(rsp));
    pr_debug_str_msg("3gpp: ", res == 0 ? "match" : "nomatch");
    return res == 0;
}

static int _3gpp_send_command_wait_response(DeviceConfig *config, const char *cmd, const char *rsp, size_t wait)
{
    _3gpp_flush(config);
    _3gpp_puts(config, cmd);
    put_crlf(config->serial);
    return _3gpp_read_response(config, rsp, wait);
}

static int _3gpp_send_command_wait(DeviceConfig *config, const char *cmd, size_t wait)
{
    return _3gpp_send_command_wait_response(config, cmd, "OK", COMMAND_WAIT);
}

static int _3gpp_send_command(DeviceConfig *config, const char * cmd)
{
    return _3gpp_send_command_wait(config, cmd, COMMAND_WAIT);
}

static int _3gpp_process_incoming(Serial *serial, char * buffer, size_t buffer_size)
{
    pr_debug_str_msg("3gpp: cmd: ", buffer);
    char *data_start = NULL;
    char *len_start = strtok_r(buffer, ",", &data_start);

    pr_debug_str_msg("buffer: " , buffer);
    pr_debug_str_msg("len: " , len_start);
    pr_debug_str_msg("data: ", data_start);

    int res = API_SUCCESS;

    return res == API_SUCCESS ? DEVICE_STATUS_OK : DEVICE_STATUS_FAULT;
}


static _3gpp_close_connection(Serial *serial, int connection_id){

}

int _3gpp_process_data(Serial * serial, char * buffer, size_t buffer_size)
{
    int res = DEVICE_STATUS_OK;
    /* check for incoming data */
    if (strstartswith(buffer, "+IPD,") == 1 ) {
        res = _3gpp_process_incoming(serial, buffer + 5, buffer_size);
    }
    else {
        pr_warning_str_msg("3gpp: unknown message: " , buffer);
    }
    return res;
}
