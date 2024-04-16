#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
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

typedef struct {
    uint64_t event_count;
} example_queue_element_t;

char digit_to_segment[] = {
    0b00111111,
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111,
    0b01110111,
    0b01111100,
    0b00111001,
    0b01011110,
    0b01111001,
    0b01110001,
    0b00000000
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

void display(char counter)
{   
    static char active_display = 0;
    char digit = active_display ? (counter >> 4) : (counter & 0x0F);
    char segment = digit_to_segment[(int)digit];
    active_display = !active_display;
    gpio_set_level(DISPLAY0, !active_display);
    gpio_set_level(DISPLAY1, active_display);
    gpio_set_level(A, (segment & 0b0000001));
    gpio_set_level(B, (segment & 0b0000010));
    gpio_set_level(C, (segment & 0b0000100));
    gpio_set_level(D, (segment & 0b0001000));
    gpio_set_level(E, (segment & 0b0010000));
    gpio_set_level(F, (segment & 0b0100000));
    gpio_set_level(G, (segment & 0b1000000));
}

char counter = 0; // Counter value
short int timer = 0; // Timer value

static bool callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_data;
    // Retrieve count value and send to queue
    example_queue_element_t ele = {
        .event_count = edata->count_value
    };
    xQueueSendFromISR(queue, &ele, &high_task_awoken);
    // return whether we need to yield at the end of ISR
    return (high_task_awoken == pdTRUE);
}

void start_timer(example_queue_element_t ele, QueueSetHandle_t queue) {
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // 1MHz, 1 tick=1us
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));
    gptimer_event_callbacks_t cbs = {
        .on_alarm = callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 10000, // period = 10ms
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

void app_main(void)
{
    // Configure the I/O ports
    configure_io_ports();

    // Configure queue
    example_queue_element_t ele;
    QueueHandle_t queue = xQueueCreate(10, sizeof(example_queue_element_t));
    if (!queue) {
        return;
    }

    // Start timer
    start_timer(ele, queue);

    while (true) {
        if (xQueueReceive(queue, &ele, pdMS_TO_TICKS(2000))) {
            // At every 100 loops (100 * 10ms = 1s), increment the counter
            if (timer++ == 100) {
                timer = 0;
                counter += 0b1;
                printf("\r%02x", counter);
                fflush(stdout);
            }
            // Display the counter value on the 7-segment display
            display(counter);
        }
    }
}
