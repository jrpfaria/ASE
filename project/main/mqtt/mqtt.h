#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "../ota/ota.h"

#define TEMP_TOPIC "temperature"
#define PRESS_TOPIC "pressure"
#define HUM_TOPIC "humidity"
#define FORECAST_TOPIC "forecast"

void mqtt_init(void);
void mqtt_publish(char *topic, char *data);
bool mqtt_is_connected();