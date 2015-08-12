#include "3gpp.h"
#include "mod_string.h"
#include "printk.h"
#include "devices_common.h"
#include "stringutil.h"
#include "modem.h"
#include "modp_numtoa.h"
#include "modp_atonum.h"
#include "api.h"

#define COMMAND_WAIT    600

static struct _3gppConnection _3gppConnections[MAX_3GPP_CONNECTIONS];

static void _3gpp_set_connection_state(size_t connection_id, bool active)
{
	if (connection_id < MAX_3GPP_CONNECTIONS) {
		pr_debug_int_msg("set connection state ", active);
		_3gppConnections[connection_id].active = active;
	}
}

/*
 * Process incoming data on the socket connection
 * expected format of data is: <connection_id>,<length>:<data>
 */
static int _3gpp_process_incoming(DeviceConfig config)
{
	char * buffer = config.buffer;
	pr_debug_str_msg("3gpp: cmd: ", buffer);
    char *data_more = NULL;
    char *connection_id_tok = strtok_r(buffer, ",", &data_more);
    char *len_tok = strtok_r(NULL, ":", &data_more);
    char *data = len_tok + strlen(len_tok) + 1;

    pr_debug_str_msg("conn_id: " , connection_id_tok);
    pr_debug_str_msg("len: " , len_tok);
    pr_debug_str_msg("data: ", data);

    int connection_id = modp_atoi(connection_id_tok);
    int prepare_rc = _3gpp_prepare_send(config, connection_id);
    if (! prepare_rc) {
    	pr_debug("failed prepare\r\n");
    	_3gpp_set_connection_state(connection_id, false);
    	return DEVICE_STATUS_FAULT;
    }

    pr_debug("processing api\r\n");
    process_api(config.serial, data);
    int send_rc = _3gpp_complete_send(config, connection_id);

    return send_rc ? DEVICE_STATUS_OK : DEVICE_STATUS_FAULT;
}

int _3gpp_prepare_send(DeviceConfig config, int connection_id)
{
	Serial *serial = config.serial;
    modem_flush(&config);
    serial->put_s("AT+CIPSENDEX=");
    put_int(serial, connection_id);
    serial->put_s(",2048"); /* maximum send size */
    put_crlf(serial);

    /* wait for the '> ' prompt */
    char c = 0;
    /* first wait for the > character */
    while(1) {
    	int rc = serial->get_c_wait(&c, COMMAND_WAIT);
    	if (rc == 0) return 0;
    	if (c == '>') break;
    }
    /* wait for the final space after > character */
    serial->get_c_wait(&c, COMMAND_WAIT);
    return c == ' ';
}

int _3gpp_complete_send(DeviceConfig config, int connection_id)
{
	pr_debug("send complete");
	/* signal send is complete by sending null char */
	config.serial->put_c('\0');
	int rc = modem_read_response(&config, "OK", COMMAND_WAIT, 0);
	if (!rc) {
		_3gpp_set_connection_state(connection_id, false);
	}
	return rc;
}

int _3gpp_close_connection(DeviceConfig *config, int connection_id){
	pr_debug_int_msg("3gpp: closing: ", connection_id);
	int rc = modem_set_value1(config, "AT+CIPCLOSE=", connection_id);
	return rc;
}

int _3gpp_process_data(DeviceConfig config)
{
	char * buffer = config.buffer;
    int res = DEVICE_STATUS_OK;
    /* check for incoming data
     * expected format is +IPD,<connection_id>,<length>:<data>
     */
    if (strstartswith(buffer, "+IPD,") == 1 ) {
    	config.buffer+=5; /* pass in the remaining data */
        res = _3gpp_process_incoming(config);
    }
/*    else {
    	process a new link / client connection
    } */
    else {
        pr_warning_str_msg("3gpp: unknown message: " , buffer);
    }
    return res;
}
