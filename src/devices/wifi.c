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
#include "modem.h"

#define WIFI_INIT_DELAY 250
#define COMMAND_WAIT    600
static wifi_status_t g_wifi_status = WIFI_STATUS_NOT_INIT;

wifi_status_t wifi_get_status()
{
    return g_wifi_status;
}

static const char * baud_config_command_for_rate(unsigned int baudRate)
{
    switch (baudRate) {
    case 9600:
        return "AT+CIOBAUD=9600";
        break;
    case 115200:
        return "AT+CIOBAUD=115200";
        break;
    case 230400:
        return "AT+CIOBAUD=230400";
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
    if (!modem_send_command(config, baud_config_command_for_rate(targetBaud)))
        return -1;
    config->serial->init(8, 0, 1, targetBaud);
    return 0;
}

static int wifi_probe_baud(unsigned int probeBaud, unsigned int targetBaud, DeviceConfig *config)
{
    pr_info_int_msg("wifi: Probing baud ", probeBaud);
    config->serial->init(8, 0, 1, probeBaud);
    int rc;
    if (modem_send_command(config, "AT") && (targetBaud == probeBaud || configure_wifi_baud(config, targetBaud) == 0)) {
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
	int rc = modem_set_value1(config, "AT+CWMODE=", wifi_mode);
	return rc;
}

static int config_access_point(DeviceConfig *config)
{
	/* TODO un-hardcode me */
    const char *ssid = "rcp";
    const char *password = "1234";
    uint8_t channel = 5;
    uint8_t encryption = 2; /* 0=OPEN; 2=WPA_PSK; 3=WPA2_PSK, 4=WPA_WPA2_PSK */

	Serial *serial = config->serial;
    modem_flush(config);
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
    return modem_read_response(config, "OK", COMMAND_WAIT);
}

static int reset_wifi(DeviceConfig *config)
{
	int rc = modem_send_command(config, "AT+RST");
	if (!rc) return rc;
	rc = modem_send_command(config, "ATE0");
	return rc;
}

static int wifi_configure_connection(DeviceConfig *config)
{
	/* Allow multiple TCP connections */
	int rc = modem_set_value1(config, "AT+CIPMUX=", 1);
	return rc;
}

static int wifi_start_service(DeviceConfig *config)
{
	/* Start TCP server on the specified port */
	/* Format is AT+CIPSERVER= <mode>[,<port>] */
	int port = 4444; /* TODO un-hardcode me */
	int rc = modem_set_value2(config, "AT+CIPSERVER=", 1, port);
	return rc;
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
    unsigned int target_baud = 230400;

    //give a chance for wifi module to init
    delayMs(WIFI_INIT_DELAY);

    // Zero terminated
    const int rates[] = { 115200, 9600, 230400, 0 };
    const int *rate = rates;
    for (; *rate != 0; ++rate) {
        const int status = wifi_probe_baud(*rate, target_baud, config);
        if (status == 0)
            break;
    }

    if (*rate > 0) {
        pr_info("wifi: detected\r\n");
        config->serial->init(8, 0, 1, target_baud);
    } else {
        pr_info("wifi: failed to detect module\r\n");
        g_wifi_status = WIFI_STATUS_ERROR;
        return DEVICE_INIT_FAIL;
    }

    int rc = config_wifi(config);
    if (rc != 0) {
    	return DEVICE_INIT_FAIL;
    }

    g_wifi_status = WIFI_STATUS_PROVISIONED;
    return DEVICE_INIT_SUCCESS;
}

int wifi_check_connection_status(DeviceConfig *config)
{
    return DEVICE_STATUS_OK;
}
