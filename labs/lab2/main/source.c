#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#define A 2
#define B 3
#define C 8
#define D 5
#define E 4
#define F 7
#define G 6
#define DP 10
#define DISPLAY0 0
#define DISPLAY1 1

static const char bin7seg_lut[] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001  // F
};

void configure_io_ports(void)
{
    // Configure the GPIOs for the 7-segment display
    gpio_reset_pin(A);
    gpio_set_direction(A, GPIO_MODE_OUTPUT);

    gpio_reset_pin(B);
    gpio_set_direction(B, GPIO_MODE_OUTPUT);

    gpio_reset_pin(C);
    gpio_set_direction(C, GPIO_MODE_OUTPUT);

    gpio_reset_pin(D);
    gpio_set_direction(D, GPIO_MODE_OUTPUT);

    gpio_reset_pin(E);
    gpio_set_direction(E, GPIO_MODE_OUTPUT);

    gpio_reset_pin(F);
    gpio_set_direction(F, GPIO_MODE_OUTPUT);

    gpio_reset_pin(G);
    gpio_set_direction(G, GPIO_MODE_OUTPUT);

    // Configure the GPIOs for the decimal point
    gpio_reset_pin(DP);
    gpio_set_direction(DP, GPIO_MODE_OUTPUT);

    // Configure the GPIOs for the display select
    gpio_reset_pin(DISPLAY0);
    gpio_set_direction(DISPLAY0, GPIO_MODE_OUTPUT);

    gpio_reset_pin(DISPLAY1);
    gpio_set_direction(DISPLAY1, GPIO_MODE_OUTPUT);
}

void display_digits(char counter, char decimal_point)
{
    // Active display flag
    static char active_display = 0;

    // Swap the active display
    active_display = !active_display;
    gpio_set_level(DISPLAY0, !active_display);
    gpio_set_level(DISPLAY1, active_display);

    // Get the 7-segment code for the digit according to which display is active
    char segment = bin7seg_lut[active_display ? (counter >> 4) : (counter & 0x0F)];
    
    // Set the GPIOs for the 7-segment display
    gpio_set_level(A, (segment & 0b0000001));
    gpio_set_level(B, (segment & 0b0000010));
    gpio_set_level(C, (segment & 0b0000100));
    gpio_set_level(D, (segment & 0b0001000));
    gpio_set_level(E, (segment & 0b0010000));
    gpio_set_level(F, (segment & 0b0100000));
    gpio_set_level(G, (segment & 0b1000000));

    // Set the GPIOs for the decimal point
    gpio_set_level(DP, active_display ? (decimal_point >> 1) : (decimal_point & 0b1));
}

void app_main(void)
{
    // Configure the I/O ports
    configure_io_ports();
    char counter = 0; // Counter value
    short int timer = 0; // Timer value
    char decimal_point = 0b00; // Decimal point states
    
    while (1) {
        // At every 100 loops (100 * 10ms = 1s), increment the counter
        if (++timer == 100) {
            timer = 0;
            counter++;
        }

        // Display the counter value on the 7-segment display 
        display_digits(counter, decimal_point);        

        // 1 second delay
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
