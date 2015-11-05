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

#include "SlipAngleTest.hh"
#include "SlipAngleTaskTest.hh"
#include "gps.h"

/* Lets see how well this works.  Allows me to keep statics */
//extern "C" {
#include "slip_angle.c"
//}

CPPUNIT_TEST_SUITE_REGISTRATION( SlipAngleTest );

void SlipAngleTest::test_init()
{
        body_heading = last_heading_gps = -1;

        init_slip_angle();

        CPPUNIT_ASSERT_EQUAL((float) 0, body_heading);
        CPPUNIT_ASSERT_EQUAL((float) 0, last_heading_gps);
}

void SlipAngleTest::test_wrap_degrees()
{
        CPPUNIT_ASSERT_EQUAL((float) 359, wrap_degrees(359));
        CPPUNIT_ASSERT_EQUAL((float) 360, wrap_degrees(360));
        CPPUNIT_ASSERT_EQUAL((float) 1, wrap_degrees(361));

        CPPUNIT_ASSERT_EQUAL((float) 1, wrap_degrees(1));
        CPPUNIT_ASSERT_EQUAL((float) 360, wrap_degrees(0));
        CPPUNIT_ASSERT_EQUAL((float) 359, wrap_degrees(-1));
}

void SlipAngleTest::test_wrap_degrees_delta()
{
        CPPUNIT_ASSERT_EQUAL((float) -179, wrap_degrees_delta(-179));
        CPPUNIT_ASSERT_EQUAL((float) -180, wrap_degrees_delta(180));
        CPPUNIT_ASSERT_EQUAL((float) -181, wrap_degrees_delta(179));

        CPPUNIT_ASSERT_EQUAL((float) 179, wrap_degrees_delta(179));
        CPPUNIT_ASSERT_EQUAL((float) 180, wrap_degrees_delta(180));
        CPPUNIT_ASSERT_EQUAL((float) 181, wrap_degrees_delta(-179));
}

void SlipAngleTest::test_going_straight() {
        CPPUNIT_ASSERT(going_straight(42, 42));

        CPPUNIT_ASSERT(going_straight(42 + HEADING_SLOP, 42));
        CPPUNIT_ASSERT(!going_straight(42 + HEADING_SLOP + .01, 42));

        CPPUNIT_ASSERT(going_straight(42 - HEADING_SLOP, 42));
        CPPUNIT_ASSERT(!going_straight(42 - HEADING_SLOP - .01, 42));
}

void SlipAngleTest::test_gps_update_no_heading()
{
        GpsSnapshot gss;
        gss.heading = 0;
        last_heading_gps = body_heading = 42;

        slip_angle_gps_update(&gss);

        CPPUNIT_ASSERT_EQUAL(gss.heading, last_heading_gps);
        CPPUNIT_ASSERT_EQUAL((float) 0, body_heading);
}

void SlipAngleTest::test_gps_update_no_last_heading()
{
        GpsSnapshot gss;
        gss.heading = 42;
        last_heading_gps = 0;
        body_heading = 43;

        slip_angle_gps_update(&gss);

        CPPUNIT_ASSERT_EQUAL(gss.heading, last_heading_gps);

        /* This just checks that there is no change. */
        CPPUNIT_ASSERT_EQUAL((float) 43, body_heading);
}

void SlipAngleTest::test_gps_update_going_straight()
{
        GpsSnapshot gss;
        gss.heading = 42;
        last_heading_gps = gss.heading;
        body_heading = 43;

        /* Called like this will cause noise measurement of yaw gyro */
        slip_angle_gps_update(&gss);

        CPPUNIT_ASSERT_EQUAL(gss.heading, last_heading_gps);

        /* This just checks that there is no change. */
        CPPUNIT_ASSERT_EQUAL((float) 43, body_heading);
}

void SlipAngleTest::test_gps_update_not_going_straight()
{
        GpsSnapshot gss;
        const float yaw_delta = -5;
        gss.heading = 52;
        last_heading_gps = 42;
        body_heading = 0;
        set_yaw_delta(yaw_delta);

        /* Called like this will cause body_heading to update */
        slip_angle_gps_update(&gss);

        CPPUNIT_ASSERT_EQUAL(gss.heading, last_heading_gps);

        /* This just checks that there is no change. */
        CPPUNIT_ASSERT_EQUAL(gss.heading + yaw_delta, body_heading);
}
