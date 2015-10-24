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

#include "gps.h"
#include "loggerConfig.h"
#include "printk.h"
#include "slip_angle.h"
#include "slip_angle_task.h"

#include <stdbool.h>
#include <math.h>

#define GPS_DEG_SLOP	1
#define INTGRL_DEG_SLOP	1
#define LOG_PFX	"[slip_angle] "

static float body_heading;
static float last_heading_gps;

bool init_slip_angle()
{
        body_heading = 0;
        last_heading_gps = 0;

        pr_info(LOG_PFX "initialized.\r\n");
        return true;
}

/**
 * Used for wrapping headings.  In example, -1 becomes 359 as -1 is not
 * a valid heading.
 */
static float wrap_degrees(float delta)
{
        if (delta > 360)
                delta -= 360;
        if (delta <= 0)
                delta += 360;

        return delta;
}

/**
 * Used for wrapping the delta between two headings, which can never
 * exceed 180.
 */
static float wrap_degrees_delta(float delta)
{
        if (delta <= -180)
                delta += 360;
        if (delta > 180)
                delta -= 360;

        return delta;
}

static bool driving_straight(const float heading_gps, const float yaw_delta)
{
        const float gps_delta =
                wrap_degrees_delta(heading_gps - last_heading_gps);
        return fabs(gps_delta) <= GPS_DEG_SLOP &&
                fabs(yaw_delta) <= INTGRL_DEG_SLOP;
}

void slip_angle_gps_update(const GpsSnapshot *gps_ss)
{
        const float heading_gps = gps_ss->heading;

        if (heading_gps == 0) {
                /* No valid GPS heading.  Can't calculate body heading. */
                body_heading = 0;
        } else if (last_heading_gps != 0) {
                /*
                 * Then we have a good current and last heading.  So we
                 * can see if we are driving straight or not.
                 */
                const float yaw_delta = get_yaw_delta();
                if (driving_straight(heading_gps, yaw_delta)) {
                        /*
                         * Reset integral value to current heading to
                         * eliminate noise in the yaw channel.
                         */
                        reset_yaw_delta();
                } else {
                        body_heading = wrap_degrees(heading_gps + yaw_delta);
                }
        }

        last_heading_gps = heading_gps;
}

float get_slip_angle(void)
{
        if (last_heading_gps && body_heading)
                return wrap_degrees_delta(body_heading - last_heading_gps);

        return 0;
}
