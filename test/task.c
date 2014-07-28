#include "task.h"

#include <sys/time.h>
#include <time.h>

portTickType xTaskGetTickCount() {
   struct timeval tp;
   gettimeofday(&tp, NULL);

   // This may be insane.  May need to use long long here.  But lazy...
   return (portTickType) (tp.tv_sec * 1000 + tp.tv_usec / 1000);
}
