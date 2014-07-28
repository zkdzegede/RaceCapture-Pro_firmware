#include "task.h"
#include "taskUtil.h"

/*
 * In my world here xTaskGetTickCount == milliseconds since epoch.  Hence
 * this little trick here works well.
 */

int isTimeoutMs(unsigned int startTicks, unsigned int timeoutMs) {
   portTickType now = xTaskGetTickCount();
   return now >= startTicks + timeoutMs;
}
