#include "stringutil.h"

int strstartswith(const char * __restrict string, const char * __restrict prefix)
{
    while(*prefix)
    {
        if(*prefix++ != *string++)
            return 0;
    }
    return 1;
}
