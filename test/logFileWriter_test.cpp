#include "logFileWriter_test.h"
#include "logFileWriter.h"
#include "sdcard_mock.h"
#include "logFileWriter.h"

#include <stdlib.h>
#include <string.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( LogFileWriterTest );

void LogFileWriterTest::setUp()
{
   UnmountFS();
}


void LogFileWriterTest::tearDown()
{
}


void LogFileWriterTest::testFileOpen()
{
  struct file_data fd;
  const char testFilePath[] = "testFile";

  CPPUNIT_ASSERT(!isMounted());

  int rc = open_file(&fd, testFilePath);
  CPPUNIT_ASSERT(isMounted());
  CPPUNIT_ASSERT_EQUAL((int) FR_OK, rc);
  char msg[120];
  snprintf(msg, sizeof(msg), "Test file path is \"%s\"\nFile Data file path is \"%s\n",
          testFilePath, fd.fPath);
  CPPUNIT_ASSERT_MESSAGE(msg, 0 == strcmp(fd.fPath, testFilePath));
}
