#include "3gpp.h"
#include "mod_string.h"
#include "printk.h"
#include "devices_common.h"
#include "stringutil.h"
#include "modem.h"
#include "modp_numtoa.h"

#define COMMAND_WAIT    600

static struct _3gppConnection _3gppConnections[MAX_3GPP_CONNECTIONS];

static int _3gpp_process_incoming(DeviceConfig *config, char * buffer, size_t buffer_size)
{
    const Serial * serial = config->serial;
	pr_debug_str_msg("3gpp: cmd: ", buffer);
    char *data_start = NULL;
    char *len_start = strtok_r(buffer, ",", &data_start);

    pr_debug_str_msg("buffer: " , buffer);
    pr_debug_str_msg("len: " , len_start);
    pr_debug_str_msg("data: ", data_start);

    int res = API_SUCCESS;

    return res == API_SUCCESS ? DEVICE_STATUS_OK : DEVICE_STATUS_FAULT;
}


static int _3gpp_close_connection(DeviceConfig *config, int connection_id){

	const Serial * serial = config->serial;
	pr_debug_int_msg("3gpp: closing: ", connection_id);
	_3gpp_puts("AT+CIPCLOSE=");
	char buf[20];
	modp_itoa10(connection_id, buf);
	_3gpp_puts(serial, buf);
	_3gpp_puts("\r");
	int rc = _3gpp_read_response(config, "OK", COMMAND_WAIT);
	return rc;
}

int _3gpp_process_data(DeviceConfig *config, char * buffer, size_t buffer_size)
{
	Serial *serial = config->serial;
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
