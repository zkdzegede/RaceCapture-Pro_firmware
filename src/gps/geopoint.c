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

#include "geopoint.h"
#include "gps.h"

#include <math.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * Converts a given value to radians.
 * @param val The provided value in degree form (0 - 360).
 * @return The radian value.
 */
static float toRad(float val)
{
    return val * (M_PI / 180.0);
}

/**
 * Converts a given value to degrees.
 * @param val The provided value in radian form.
 * @return The degree value.
 */
static float toDeg(float val)
{
    return val * (180.0 / M_PI);
}

float distPythag(const GeoPoint *a, const GeoPoint *b)
{
    const float dLatRad = toRad(b->latitude - a->latitude);
    const float dLonRad = toRad(b->longitude - a->longitude);
    const float latARad = toRad(a->latitude);
    const float latBRad = toRad(b->latitude);

    const float tmp = dLonRad * cos((latARad + latBRad) / 2);

    return sqrt(tmp * tmp + dLatRad * dLatRad) * GP_EARTH_RADIUS_M;
}

float gps_bearing(const GeoPoint *last, const GeoPoint *curr)
{
        /*
         * Algorithm adapted from
         * http://www.movable-type.co.uk/scripts/latlong.html
         * Had to use this one as bearing is a relative calculation.
         * Hopefully this doesn't overflow the float values.
         */

        const float d_lon = toRad(curr->longitude) - toRad(last->longitude);
        const float lat_curr = toRad(curr->latitude);
        const float lat_last = toRad(last->latitude);

        const float y = sin(d_lon) * cos(lat_curr);
        const float x = cos(lat_last) * sin(lat_curr) -
                sin(lat_last) * cos(lat_curr) * cos(d_lon);

        float res = toDeg(atan2(y, x));
        return res < 0 ? res + 360 : res;
}

int isValidPoint(const GeoPoint *p)
{
    return p->latitude != 0.0 || p->longitude != 0.0;
}

bool are_geo_points_equal(const GeoPoint *p1, const GeoPoint *p2)
{
        if (NULL == p1 || NULL == p2)
                return p1 == p2;

        return p1->latitude == p2->latitude &&
                p1->longitude == p2->longitude;
}
