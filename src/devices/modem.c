/**
 * Race Capture Pro Firmware
 *
 * Copyright (C) 2015 Autosport Labs
 *
 * This file is part of the Race Capture Pro fimrware suite
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should
 * have received a copy of the GNU General Public License along with
 * this code. If not, see <http://www.gnu.org/licenses/>.
 */

#include "modem.h"
#include "printk.h"
#define COMMAND_WAIT    600

int modem_read_wait(DeviceConfig *config, size_t delay)
{
    int c = config->serial->get_line_wait(config->buffer, config->length, delay);
    return c;
}

void modem_flush(DeviceConfig *config)
{
    config->buffer[0] = '\0';
    config->serial->flush();
}

void modem_puts(DeviceConfig *config, const char *data)
{
    config->serial->put_s(data);
}

int modem_read_response(DeviceConfig *config, const char *rsp, size_t wait)
{
    modem_read_wait(config, wait);
    pr_debug_str_msg("modem: cmd rsp: ", config->buffer);
    int res = strstr(config->buffer, rsp);
    res = res != NULL;
    return res;
}

int modem_send_command_wait_response(DeviceConfig *config, const char *cmd, const char *rsp, size_t wait)
{
    modem_flush(config);
    modem_puts(config, cmd);
    put_crlf(config->serial);
    int rc = modem_read_response(config, rsp, wait);
    if (!rc) {
    	pr_error("modem: command failed: ");
    	pr_error(cmd);
    	pr_error_str_msg(" ; rsp:", rsp);
    }
    return rc;
}

int modem_send_command_wait(DeviceConfig *config, const char *cmd, size_t wait)
{
    return modem_send_command_wait_response(config, cmd, "OK", COMMAND_WAIT);
}

int modem_send_command(DeviceConfig *config, const char * cmd)
{
    return modem_send_command_wait(config, cmd, COMMAND_WAIT);
}

int modem_set_value1(DeviceConfig *config, const char * command, int value1)
{
	Serial *serial = config->serial;
    modem_flush(config);
    serial->put_s(command);
    put_int(serial, value1);
    put_crlf(serial);
    int rc = modem_read_response(config, "OK", COMMAND_WAIT);
    return rc;
}

int modem_set_value2(DeviceConfig *config, const char * command, int value1, int value2)
{
	Serial *serial = config->serial;
    modem_flush(config);
    serial->put_s(command);
    put_int(serial, value1);
    serial->put_s(",");
    put_int(serial, value2);
    put_crlf(serial);
    int rc = modem_read_response(config, "OK", COMMAND_WAIT);
    return rc;
}
