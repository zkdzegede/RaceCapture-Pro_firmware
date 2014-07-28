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
  CPPUNIT_ASSERT(fd.file_status & FS_OPEN);

  char msg[120];
  snprintf(msg, sizeof(msg), "Test file path is \"%s\"; File Data file path is \"%s\n",
          testFilePath, fd.fPath);
  CPPUNIT_ASSERT_MESSAGE(msg, 0 == strcmp(fd.fPath, testFilePath));
  close_file(&fd);
}


void LogFileWriterTest::testFileClose()
{
  struct file_data fd;
  const char testFilePath[] = "testFile";

  int rc = open_file(&fd, testFilePath);

  CPPUNIT_ASSERT(isMounted());
  CPPUNIT_ASSERT_EQUAL((int) FR_OK, rc);
  CPPUNIT_ASSERT(fd.file_status & FS_OPEN);

  rc = close_file(&fd);

  CPPUNIT_ASSERT(isMounted());
  CPPUNIT_ASSERT_EQUAL((int) FR_OK, rc);
  CPPUNIT_ASSERT((fd.file_status & FS_OPEN) == 0);
  char msg[120];
  snprintf(msg, sizeof(msg), "Path should be null but File Data file path is \"%s\"\n",
           fd.fPath);
  CPPUNIT_ASSERT_MESSAGE(msg, 0 == strcmp(fd.fPath, ""));
}

static int startsWith(const char *str, const char *prefix)
{
  if (!str || !prefix) return 0;
  size_t len = strlen(prefix);

  return strncmp(str, prefix, len) == 0;
}

static int endsWith(const char *str, const char *suffix)
{
  if (!str || !suffix) return 0;

  size_t lenstr = strlen(str);
  size_t lensuffix = strlen(suffix);

  if (lensuffix >  lenstr) return 0;
  return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

void LogFileWriterTest::testFileAppendNew()
{
  struct file_data fd;
  fd.pfx = "test_";
  fd.sfx = ".tLog";

  int rc = append_to_file(&fd, "Some Test Data");

  CPPUNIT_ASSERT(isMounted());
  CPPUNIT_ASSERT_EQUAL((int) FR_OK, rc);
  CPPUNIT_ASSERT(fd.file_status & FS_OPEN);
  CPPUNIT_ASSERT(strlen(fd.fPath) > 0);

  char msg[120];
  snprintf(msg, sizeof(msg), "Prefix is \"%s\", Path is \"%s\"\n",
          fd.pfx, fd.fPath);
  CPPUNIT_ASSERT_MESSAGE(msg, startsWith(fd.fPath, fd.pfx));

  snprintf(msg, sizeof(msg), "Suffix is \"%s\", Path is \"%s\"\n",
          fd.sfx, fd.fPath);
  CPPUNIT_ASSERT_MESSAGE(msg, endsWith(fd.fPath, fd.sfx));

  close_file(&fd);
}
