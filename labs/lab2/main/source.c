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

char digit_to_segment(char digit)
{
    switch (digit)
    {
        case 0x0:
            return 0b00111111;
        case 0x1:
            return 0b00000110;
        case 0x2:
            return 0b01011011;
        case 0x3:
            return 0b01001111;
        case 0x4:
            return 0b01100110;
        case 0x5:
            return 0b01101101;
        case 0x6:
            return 0b01111101;
        case 0x7:
            return 0b00000111;
        case 0x8:
            return 0b01111111;
        case 0x9:
            return 0b01101111;
        case 0xa:
            return 0b01110111;
        case 0xb:
            return 0b01111100;
        case 0xc:
            return 0b00111001;
        case 0xd:
            return 0b01011110;
        case 0xe:
            return 0b01111001;
        case 0xf:
            return 0b01110001;
        default:
            return 0b00000000;
    }
}

static char active_display = 0;
void swap_display(void)
{
    active_display = !active_display;
    gpio_set_level(DISPLAY0, !active_display);
    gpio_set_level(DISPLAY1, active_display);
}

void display_digit(char digit)
{
    char segment = digit_to_segment(digit);
    gpio_set_level(A, (segment & 0b0000001));
    gpio_set_level(B, (segment & 0b0000010));
    gpio_set_level(C, (segment & 0b0000100));
    gpio_set_level(D, (segment & 0b0001000));
    gpio_set_level(E, (segment & 0b0010000));
    gpio_set_level(F, (segment & 0b0100000));
    gpio_set_level(G, (segment & 0b1000000));
}

void display_decimal_point(char state)
{
    gpio_set_level(DP, state);
}

void app_main(void)
{
    // Configure the I/O ports
    configure_io_ports();
    char counter = 0; // Counter value
    short int timer = 0; // Timer value
    char decimal_point = 0b00;
    
    while (1) {
        // At every 100 loops (100 * 10ms = 1s), increment the counter
        if (timer++ == 100) {
            timer = 0;
            counter += 0b1;
        }

        // Display the counter value on the 7-segment display
        display_digit(active_display ? (counter >> 4) : (counter & 0x0F));        

        // Display the decimal point if needed
        display_decimal_point(active_display ? (decimal_point >> 1) : (decimal_point & 0b1));

        // Repeat at 100Hz rate
        swap_display();
        
        // 1 second delay
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
