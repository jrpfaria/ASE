#pragma once

#include "../ota/ota.h"

#define TEMP_TOPIC "temperature"
#define PRESS_TOPIC "pressure"
#define HUM_TOPIC "humidity"
#define FORECAST_TOPIC "forecast"

void mqtt_init(void);
void mqtt_publish(char *topic, char *data);
