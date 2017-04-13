/* Martin Thomas 4/2009 */

#include "dateTime.h"
#include "ff.h"
#include "gps.h"

DWORD get_fattime (void)
{
    DateTime dt;
    getDateTimeFromEpochMillis(&dt, getLastFix());

    if (!isValidDateTime(dt))
        return (DWORD) 0;

    return (((DWORD)dt.year - 1980) << 25)
           | ((DWORD)dt.month << 21)
           | ((DWORD)dt.day << 16)
           | (WORD)(dt.hour << 11)
           | (WORD)(dt.minute << 5)
           | (WORD)(dt.second >> 1);
}
