#include "esp_log.h"
#include "esp_sntp.h"
#include <time.h>
#include <sys/time.h>

#define TAG_SNTP "SNTP"
#define RETRY_COUNT 10

void time_init(void);
char* getTimestamp(void);