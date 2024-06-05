#include "sntp.h"

static const char* TAG = TAG_SNTP;

void time_init(void) {
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org");
    esp_sntp_init();
    time_t now;
    struct tm timeinfo;
    int retry = 0;
    while (timeinfo.tm_year < (2016 - 1900) && ++retry < RETRY_COUNT){
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, RETRY_COUNT);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (retry == RETRY_COUNT) {
        ESP_LOGE(TAG, "Failed to synchronize time with SNTP server");
    }

    time(&now);
    localtime_r(&now, &timeinfo);

    // Set timezone to WET (Western European Time) and DST to WEST (Western European Summer Time)
    setenv("TZ", "WET0WEST,M3.5.0/1,M10.5.0/2", 1);
    tzset();

    time(&now);
    localtime_r(&now, &timeinfo);
}

char* getTimestamp(void) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    // adjust_time(now);
    localtime_r(&now, &timeinfo);

    static char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return time_str;
}