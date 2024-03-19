#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#define a_segment 
#define b_segment
#define c_segment
#define d_segment
#define e_segment
#define f_segment
#define g_segment
#define dp_segment
#define display_1
#define display_2

void configure_io_ports(void)
{
    // Configure the input port
    gpio_reset_pin(INPUT_GPIO);
    gpio_set_direction(INPUT_GPIO, GPIO_MODE_INPUT);
    // Configure the output port
    gpio_reset_pin(OUTPUT_GPIO);
    gpio_set_direction(OUTPUT_GPIO, GPIO_MODE_OUTPUT);
}

void app_main(void)
{
    // Configure the I/O ports
    configure_io_ports();

    while (1) {
        // Set the output port to the value of the input port
        gpio_set_level(OUTPUT_GPIO, gpio_get_level(INPUT_GPIO));
    }
}
