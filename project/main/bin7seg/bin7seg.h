#include "driver/gpio.h"

#define A 2
#define B 3
#define C 8
#define D 5
#define E 4
#define F 7
#define G 6
#define DISPLAY 10

#define configure_pin(x)                                        \
    do                                                          \
    {                                                           \
        gpio_reset_pin(x);                                      \
        gpio_set_direction(x, GPIO_MODE_OUTPUT);                \
    } while (0);

char* getWeatherState(int Z);
int char2seg(char c);
void configure_io_ports();
void displayStatus(char* status);