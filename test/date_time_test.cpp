/**
 * Race Capture Pro Firmware
 *
 * Copyright (C) 2014 Autosport Labs
 *
 * This file is part of the Race Capture Pro fimrware suite
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
 *
 * Authors: Stieg
 */

#include "dateTime.h"
#include "date_time_test.h"

#include <cppunit/extensions/HelperMacros.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DateTimeTest );

void DateTimeTest::setUp() {}

void DateTimeTest::tearDown() {}

const DateTime g_invalid_dt = {};

// Jan 1, 2000.  Happy New Year!
const DateTime g_new_millenium = {0, 0, 0, 0, 1, 1, 0};

void DateTimeTest::testIsValidDateTime()
{
   CPPUNIT_ASSERT(!isValidDateTime(g_invalid_dt));
   CPPUNIT_ASSERT(isValidDateTime(g_new_millenium));
}

void DateTimeTest::testGetMillisSinceEpoch() {
   // 1970, Jan 1 @ 00:00:00.000 == 0
   const DateTime epoch = {0, 0, 0, 0, 1, 1, 0};
   CPPUNIT_ASSERT_EQUAL((unsigned long) 0,
                        getMillisecondsSinceUnixEpoch(epoch));

   // 1970, Jan 1 @ 00:16:40.000 == 1000000
   const DateTime d1000000 = {0, 0, 40, 16, 1, 1, 0};
   CPPUNIT_ASSERT_EQUAL((unsigned long) 1000000,
                        getMillisecondsSinceUnixEpoch(d1000000));

   // 2013, January 1 @ 00:00:00.000 == 1356998400000 milliseconds since epoch.
   const DateTime jan_1_2013 = {0, 0, 0, 0, 1, 1, 13};
   CPPUNIT_ASSERT_EQUAL((unsigned long) 1356998400000,
                        getMillisecondsSinceUnixEpoch(jan_1_2013));

}

void DateTimeTest::testGetDeltaInMillis() {}
