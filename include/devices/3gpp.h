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

#ifndef _3GPP_H_
#define _3GPP_H_

#include <stddef.h>
#include <stdint.h>
#include "serial.h"
#include "api.h"
#include <stdbool.h>

#define MAX_3GPP_CONNECTIONS 10

struct _3gppConnection {
    bool active;
};

int _3gpp_process_data(Serial * serial, char * buffer, size_t buffer_size);

#endif /* _3GPP_H_ */
