/*
 * logFileWriter_test.h
 *
 *  Created on: July 25, 2014
 *      Author: stieg
 */

#ifndef LOG_FILE_WRITER_TEST_H_
#define LOG_FILE_WRITER_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

class LogFileWriterTest : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE( LogFileWriterTest );
  CPPUNIT_TEST( testFileOpen );
  CPPUNIT_TEST( testFileClose );
  CPPUNIT_TEST( testFileAppendNew );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();

  void testFileOpen();
  void testFileClose();
  void testFileAppendNew();
};

#endif /* LOG_FILE_WRITER_TEST_H_ */
