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
#include "devices_common.h"

#ifndef MODEM_H_

int modem_read_wait(DeviceConfig *config, size_t delay);

void modem_flush(DeviceConfig *config);

void modem_puts(DeviceConfig *config, const char *data);

int modem_read_response(DeviceConfig *config, const char *rsp, size_t wait, uint8_t echo_mode);

int modem_send_command_wait_response(DeviceConfig *config, const char *cmd, const char *rsp, size_t wait, uint8_t echo_mode);

int modem_send_command_wait(DeviceConfig *config, const char *cmd, size_t wait, uint8_t echo_mode);

int modem_send_command(DeviceConfig *config, const char * cmd);

int modem_set_value1(DeviceConfig *config, const char * command, int value1);

int modem_set_value2(DeviceConfig *config, const char * command, int value1, int value2);

#define MODEM_H_


#endif
