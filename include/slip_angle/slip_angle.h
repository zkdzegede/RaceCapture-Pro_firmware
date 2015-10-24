/*
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

#ifndef _SLIP_ANGLE_H_
#define _SLIP_ANGLE_H_

#include "gps.h"

/**
 * Initializes all of the slip angle state variables.
 */
bool init_slip_angle(void);

/**
 * Called when a new GpsSnapshot is available.  This task will in turn
 * update the slip angle calculations as needed.
 */
void slip_angle_gps_update(const GpsSnapshot *gps_ss);

/**
 * Gets the slip angle (in degrees) of the vehicle relative to the direction
 * of travel.  A positive value means the rotation of the vehicle is clockwise
 * of the direction of travel.  A negative value means the rotation of the
 * vehicle is anti-clockwise to the direction of travel.
 */
float get_slip_angle(void);

#endif /* _SLIP_ANGLE_H_ */
