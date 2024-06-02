#include "bme280.h"
#include "esp_timer.h"
#include "wifi.h"
#include "mqtt.h"
#include "time.h"
#include <string.h>

#define CHECK(x)                                                \
    do                                                          \
    {                                                           \
        esp_err_t err;                                          \
        if ((err = ESP_ERROR_CHECK_WITHOUT_ABORT(x)) != ESP_OK) \
        {                                                       \
            printf("Error %d\n", err);                          \
            return;                                             \
        }                                                       \
    } while (0)

#define SDA 0
#define SCL 1
#define A 2
#define B 3
#define C 8
#define D 5
#define E 4
#define F 7
#define G 6
#define DISPLAY 10

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
    0b01000000};

char* getTime()
{
    time_t now;
    struct tm *utc_time;
    time(&now);
    utc_time = gmtime(&now);
    char* time_str = malloc(50);

    if (utc_time == NULL)
    {
        printf("(GetTime) Malloc failed\n");
        return NULL;
    }

    snprintf(time_str, 50, "date=%04d-%02d-%02d, time=%02d:%02d:%02d",
             utc_time->tm_year + 1900, utc_time->tm_mon + 1, utc_time->tm_mday,
             utc_time->tm_hour, utc_time->tm_min, utc_time->tm_sec);
    return time_str;
}

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

    // Configure the GPIOs for the display select
    gpio_reset_pin(DISPLAY);
    gpio_set_direction(DISPLAY, GPIO_MODE_OUTPUT);
}

void display(char value)
{
    static char active_display = 0;
    char digit = active_display ? (value >> 4) : (value & 0x0F);
    char segment = digit_to_segment[(int)digit];
    gpio_set_level(DISPLAY, active_display);
    gpio_set_level(A, (segment & 0b0000001));
    gpio_set_level(B, (segment & 0b0000010));
    gpio_set_level(C, (segment & 0b0000100));
    gpio_set_level(D, (segment & 0b0001000));
    gpio_set_level(E, (segment & 0b0010000));
    gpio_set_level(F, (segment & 0b0100000));
    gpio_set_level(G, (segment & 0b1000000));
    active_display = !active_display;
}

void display_b10(int value)
{
    static char active_display = 0;
    char digit;
    if (value >= 0)
        digit = active_display ? (value / 10) : (value % 10);
    else
        digit = active_display ? 16 : (-value % 10);
    char segment = digit_to_segment[(int)digit];
    gpio_set_level(DISPLAY, active_display);
    gpio_set_level(A, (segment & 0b0000001));
    gpio_set_level(B, (segment & 0b0000010));
    gpio_set_level(C, (segment & 0b0000100));
    gpio_set_level(D, (segment & 0b0001000));
    gpio_set_level(E, (segment & 0b0010000));
    gpio_set_level(F, (segment & 0b0100000));
    gpio_set_level(G, (segment & 0b1000000));
    active_display = !active_display;
}

i2c_master_bus_handle_t busHandle;
i2c_master_dev_handle_t sensorHandle;

bme280_comp_data_t sensorData;

short int timer = 0; // Timer value

static void callback_sensor(void *arg)
{
    uint8_t id;
    CHECK(bme280_read_id(sensorHandle, &id));
    printf("ID: %x\n", id);

    // // Read the temperature
    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    CHECK(bme280_read_data(sensorHandle, &sensorData));

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Temperature: %8.2f\t *C\n", sensorData.temperature / 100.0);
    printf("Pressure   : %8.2f\thPa\n", sensorData.pressure / 256.0 / 100.0);
    printf("Humidity   : %8.2f\t%%RH\n", sensorData.humidity / 1024.0);

    char* time = getTime();
    char* data = malloc(20);    
    if (data == NULL){
        printf("(CLB)Malloc failed\n");
        return;
    }
    snprintf(data, 20, "%5.2f", sensorData.temperature / 100.0);
    mqtt_publish(TEMP_TOPIC, data, time);
    snprintf(data, 20, "%5.2f", sensorData.pressure / 256.0 / 100.0);
    mqtt_publish(PRESS_TOPIC, data, time);
    snprintf(data, 20, "%5.2f", sensorData.humidity / 1024.0);
    mqtt_publish(HUM_TOPIC, data, time);

    free(time);
    free(data);
    // printf("\r %f %f %f ", humidity[0] | (pressure[0] << 8));
    // printf("%d", temperature[0]);
    // fflush(stdout);
}

static void callback_display(void *arg)
{
    // Display the temperature value on the 7-segment display
    // display_b10(temperature[0]);
    CHECK(bme280_set_mode(sensorHandle, MODE_FORCED));
}

void start_timers()
{
    const esp_timer_create_args_t periodic_timer_args_display = {
        .callback = &callback_display,
        .name = "periodic"};
    const esp_timer_create_args_t periodic_timer_args_sensor = {
        .callback = &callback_sensor,
        .name = "periodic"};
    esp_timer_handle_t periodic_timer_display;
    esp_timer_handle_t periodic_timer_sensor;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args_display, &periodic_timer_display));
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args_sensor, &periodic_timer_sensor));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_display, 60000000)); // 1 min
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_sensor, 10000000));   // 10s
}

void app_main(void)
{
    wifi_init();
    wifi_start();
    mqtt_init();

    // Configure the I/O ports
    configure_io_ports();

    // Configure the sensor
    uint8_t sensorAddr = 0x77;
    int sdaPin = SDA;
    int sclPin = SCL;
    uint32_t clkSpeedHz = 400000;
    CHECK(bme280_init(&busHandle, &sensorHandle, sensorAddr, sdaPin, sclPin, clkSpeedHz));

    CHECK(bme280_default_setup(sensorHandle));

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    start_timers();
}
