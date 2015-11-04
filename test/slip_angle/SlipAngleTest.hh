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

#ifndef _SLIPANGLETEST_H_
#define _SLIPANGLETEST_H_

#include <cppunit/extensions/HelperMacros.h>

class SlipAngleTest : public CppUnit::TestCase
{
        CPPUNIT_TEST_SUITE(SlipAngleTest);

        /* Tests to Run */
        CPPUNIT_TEST(test_init);
        CPPUNIT_TEST(test_wrap_degrees);
        //CPPUNIT_TEST(test_driving_straight);
        //CPPUNIT_TEST(test_not_driving_straight);
        CPPUNIT_TEST(test_gps_update_no_heading);
        CPPUNIT_TEST(test_gps_update_no_last_heading);
        //CPPUNIT_TEST(test_gps_update_going_straight);
        //CPPUNIT_TEST(test_gps_update_going_turning);

        CPPUNIT_TEST_SUITE_END();

public:
        SlipAngleTest();
        void test_init();
        void test_wrap_degrees();
        void test_wrap_degrees_delta();
        //void test_driving_straight();
        //void test_not_driving_straight();
        void test_gps_update_no_heading();
        void test_gps_update_no_last_heading();
        //void test_gps_update_going_straight();
        //void test_gps_update_going_turning();

private:
};

#endif /* _SLIPANGLETEST_H_ */
