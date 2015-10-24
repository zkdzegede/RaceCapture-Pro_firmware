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

#ifndef _SLIP_ANGLE_TASK_H_
#define _SLIP_ANGLE_TASK_H_

/**
 * Initializes the yaw config by locating the yaw channel in the loggerConfig
 * and resetting the yaw_delta.
 */
void init_slip_angle_yaw_cfg(void);

/**
 * Used to reset the yaw_delta.  Used when we know we are going straight to
 * eliminate noise from the gyro.
 */
void reset_yaw_delta(void);

/**
 * Gets the yaw_delta value.
 */
float get_yaw_delta(void);

/**
 * Starts the slip angle task.  Should only be called once during the initial
 * setup of all tasks.
 */
bool start_slip_angle_task(const int priority);

#endif /* _SLIP_ANGLE_TASK_H_ */
