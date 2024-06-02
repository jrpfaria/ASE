#pragma once

#define TEMP_TOPIC "ASEP1G4/temperature"
#define PRESS_TOPIC "ASEP1G4/pressure"
#define HUM_TOPIC "ASEP1G4/humidity"

void mqtt_init(void);
void mqtt_publish(char *topic, char *data, char* time);
