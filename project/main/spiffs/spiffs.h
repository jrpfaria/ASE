#include "esp_spiffs.h"
#include "esp_log.h"

#define MINUTES_BETWEEN_STORING_DATA 5

int spiffsUsedSpace();
void init_spiffs(FILE* f, char* file_path);