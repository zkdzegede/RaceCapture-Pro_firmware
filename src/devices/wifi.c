/**
 * AutoSport Labs - Race Capture Pro Firmware
 *
 * Copyright (C) 2014 AutoSport Labs
 *
 * This file is part of the Race Capture Pro firmware suite
 *
 * This is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with this code. If not, see <http://www.gnu.org/licenses/>.
 */

#include "wifi.h"
#include "FreeRTOS.h"
#include "loggerConfig.h"
#include "mod_string.h"
#include "printk.h"
#include "task.h"
#include "taskUtil.h"
#include "serial.h"

#define WIFI_INIT_DELAY 100
#define COMMAND_WAIT    600
static wifi_status_t g_wifi_status = WIFI_STATUS_NOT_INIT;

wifi_status_t wifi_get_status()
{
    return g_wifi_status;
}

static int read_wifi_wait(DeviceConfig *config, size_t delay)
{
    int c = config->serial->get_line_wait(config->buffer, config->length, delay);
    return c;
}

static void flush_wifi(DeviceConfig *config)
{
    config->buffer[0] = '\0';
    config->serial->flush();
}

void puts_wifi(DeviceConfig *config, const char *data)
{
    config->serial->put_s(data);
}

static int read_wifi_response(DeviceConfig *config, const char *rsp, size_t wait)
{
    read_wifi_wait(config, wait);
    pr_debug_str_msg("wifi: cmd rsp: ", config->buffer);
    int res = strncmp(config->buffer, rsp, strlen(rsp));
    pr_debug_str_msg("wifi: ", res == 0 ? "match" : "nomatch");
    return res == 0;
}

static int sendWifiCommandWaitResponse(DeviceConfig *config, const char *cmd, const char *rsp, size_t wait)
{
    flush_wifi(config);
    puts_wifi(config, cmd);
    put_crlf(config->serial);
    return read_wifi_response(config, rsp, wait);
}

static int sendWifiCommandWait(DeviceConfig *config, const char *cmd, size_t wait)
{
    return sendWifiCommandWaitResponse(config, cmd, "OK", COMMAND_WAIT);
}

static int sendCommand(DeviceConfig *config, const char * cmd)
{
    pr_debug_str_msg("wifi: cmd: ", cmd);
    return sendWifiCommandWait(config, cmd, COMMAND_WAIT);
}

static const char * baudConfigCmdForRate(unsigned int baudRate)
{
    switch (baudRate) {
    case 9600:
        return "AT+CIOBAUD=9600\n";
        break;
    case 115200:
        return "AT+CIOBAUD=115200\n";
        break;
    case 230400:
        return "AT+CIOBAUD=230400\n";
        break;
    default:
        break;
    }
    pr_error_int_msg("invalid wifi baud", baudRate);
    return "";
}

static int configure_wifi_baud(DeviceConfig *config, unsigned int targetBaud)
{
    pr_info_int_msg("wifi: Configuring baud Rate", targetBaud);
    //set baud rate
    if (!sendCommand(config, baudConfigCmdForRate(targetBaud)))
        return -1;
    config->serial->init(8, 0, 1, targetBaud);
    return 0;
}

static int wifi_probe_baud(unsigned int probeBaud, unsigned int targetBaud, DeviceConfig *config)
{
    pr_info_int_msg("wifi: Probing baud ", probeBaud);
    config->serial->init(8, 0, 1, probeBaud);
    int rc;
    if (sendCommand(config, "AT") && (targetBaud == probeBaud || configure_wifi_baud(config, targetBaud) == 0)) {
        rc = DEVICE_INIT_SUCCESS;
    } else {
        rc = DEVICE_INIT_FAIL;
    }
    pr_info_str_msg("wifi: Provision ", rc == DEVICE_INIT_SUCCESS ? "win" : "fail");
    return rc;
}

static int config_wifi_mode(DeviceConfig *config)
{

	/* TODO un-hardcode me */
    uint8_t wifi_mode = 2; /* 2=AP; 1=STA; 3=both */

	Serial *serial = config->serial;
    flush_wifi(config);
    serial->put_s("AT+CWMODE=");
    put_uint(serial, wifi_mode);
    put_crlf(serial);
    return read_wifi_response(config, "OK", COMMAND_WAIT);
}

static int config_access_point(DeviceConfig *config)
{
	/* TODO un-hardcode me */
    const char *ssid = "rcp";
    const char *password = "1234";
    uint8_t channel = 5;
    uint8_t encryption = 2; /* 0=OPEN; 2=WPA_PSK; 3=WPA2_PSK, 4=WPA_WPA2_PSK */

	Serial *serial = config->serial;
    flush_wifi(config);
    /* format is AT+CWSAP="ssid","pwd",channel,encryption */
    serial->put_s("AT+CWSAP=\"");
    serial->put_s(ssid);
    serial->put_s("\",\"");
    serial->put_s(password);
    serial->put_s(",");
    put_uint(serial, channel);
    serial->put_s(",");
    put_uint(serial, encryption);
    put_crlf(serial);

    return read_wifi_response(config, "OK", COMMAND_WAIT);
}

static int reset_wifi(DeviceConfig *config)
{
	return sendCommand(config, "AT+RST\n");
}

static int wifi_configure_connection(DeviceConfig *config)
{
	/* Allow multiple TCP connections */
	return sendCommand(config, "AT+CIPMUX=1");
}

static int wifi_start_service(DeviceConfig *config)
{
	/* Start TCP server on the specified port */
	/* Format is AT+ CIPSERVER= <mode>[,<port>] */
	int port = 14600; /* TODO un-hardcode me */

	Serial *serial = config->serial;
    flush_wifi(config);
    serial->put_s("AT+CIPSERVER=1,");
    put_uint(serial, port);
    put_crlf(serial);
    return read_wifi_response(config, "OK", COMMAND_WAIT);
}

static int config_wifi(DeviceConfig *config)
{

	if (!reset_wifi(config)) {
		return -1;
	}

    if (!config_wifi_mode(config)) {
    	return -2;
    }

    if (!wifi_configure_connection(config)) {
        return -3;
    }

    if (!config_access_point(config)) {
    	return -4;
    }

    if (!wifi_start_service(config)) {
    	return -5;
    }
	return 0;
}

int wifi_disconnect(DeviceConfig *config)
{
    return 0; //NOOP
}

int wifi_init_connection(DeviceConfig *config)
{
    //BluetoothConfig *btConfig = &(getWorkingLoggerConfig()->ConnectivityConfigs.bluetoothConfig);
    unsigned int targetBaud = 230400;

    //give a chance for wifi module to init
    delayMs(WIFI_INIT_DELAY);

    // Zero terminated
    const int rates[] = { 115200, 9600, 230400, 0 };
    const int *rate = rates;
    for (; *rate != 0; ++rate) {
        const int status = wifi_probe_baud(*rate, targetBaud, config);
        if (status == 0)
            break;
    }

    if (*rate > 0) {
        pr_info("wifi: detected\r\n");
        config->serial->init(8, 0, 1, targetBaud);
    } else {
        pr_info("wifi: Failed to detect module\r\n");
        g_wifi_status = WIFI_STATUS_ERROR;
        return DEVICE_INIT_FAIL;
    }

    if (config_wifi(config) != 0) {
    	return DEVICE_INIT_FAIL;
    }

    g_wifi_status = WIFI_STATUS_PROVISIONED;
    return DEVICE_INIT_SUCCESS;
}

int wifi_check_connection_status(DeviceConfig *config)
{
    return DEVICE_STATUS_OK;
}
