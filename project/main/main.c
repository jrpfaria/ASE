#include "bme280.h"
#include "esp_timer.h"
#include "wifi.h"
#include "mqtt.h"
#include "forecast.h"
#include "bin7seg.h"
#include <string.h>

#define SENSOR_ADDR 0x77
#define SDA_PIN 0
#define SCL_PIN 1
#define CLK_SPEED_HZ 400000

#define A 2
#define B 3
#define C 8
#define D 5
#define E 4
#define F 7
#define G 6
#define DISPLAY 10

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

#define publish(topic, x)                                       \
    do                                                          \
    {                                                           \
        snprintf(aux, 7, "%5.2f", x);                           \
        mqtt_publish(topic, aux);                               \
    } while (0)

#define configure_pin(x)                                        \
    do                                                          \
    {                                                           \
        gpio_reset_pin(x);                                      \
        gpio_set_direction(x, GPIO_MODE_OUTPUT);                \
    } while (0);
    
#define setupDisplayOnStart() memcpy(forecastToDisplay, getWeatherState(-1), 7)

// Global Variables
i2c_master_bus_handle_t busHandle;      // I2C bus handle
i2c_master_dev_handle_t sensorHandle;   // I2C device handle
bme280_comp_data_t sensorData;          // Sensor data
int sensorReadIteration = 0;            // Sensor read iteration
int forecastReady = 1;                  // Flag to indicate if the forecast is ready to be computed
char forecastData[40];                  // Forecast data
char forecastToDisplay[7];              // Forecast to display
int timer = 0;                          // Timer to control the display

void configure_io_ports()
{
    configure_pin(A);
    configure_pin(B);
    configure_pin(C);
    configure_pin(D);
    configure_pin(E);
    configure_pin(F);
    configure_pin(G);
    configure_pin(DISPLAY);
}

void displayStatus(char* status)
{
    static int active_display = 0;
    char segment = char2seg(status[active_display]);
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

void print_data()
{
    static int firstIterationFlag = 1;
    if (firstIterationFlag--)
        printf("\033[H\033[J"); // Clear the screen
    printf("\033[1;1H"); // Move cursor to line 1, column 1    
    printf("Temperature: %8.2f\t *C\n", sensorData.temperature);
    printf("Pressure   : %8.2f\thPa\n", sensorData.pressure);
    printf("Humidity   : %8.2f\t%%RH\n", sensorData.humidity);
    printf("%40s\n", forecastData);
    fflush(stdout);
}

void post_data()
{
    char aux[7];
    publish(TEMP_TOPIC, sensorData.temperature);
    publish(PRESS_TOPIC, sensorData.pressure);
    publish(HUM_TOPIC, sensorData.humidity);
    mqtt_publish(FORECAST_TOPIC, forecastData);
}

static void callback_sensor(void *arg)
{
    CHECK(bme280_set_mode(sensorHandle, MODE_FORCED));

    if (sensorReadIteration++ == 1/*MINUTES_BETWEEN_FORECASTS*/)
    {
        forecastReady = 1;
        sensorReadIteration = 0;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Wait for the sensor to have the new data

    CHECK(bme280_read_data(sensorHandle, &sensorData));

    if (forecastReady)
    {
        int forecastIndex[1];
        memcpy(forecastData, computeForecast(sensorData.temperature,
                                             sensorData.pressure, 
                                             DETI_ALTITUDE, NORTH_WINDS, SUMMER,
                                             forecastIndex), 40); 
        
        memcpy(forecastToDisplay, getWeatherState(*forecastIndex), 7);
        
        forecastReady = 0;
    }

    print_data();

    post_data();
}

static void callback_display(void *arg)
{
    static int iter = 0;
    char substring[2];
    if (timer++ == 100)
    {
        timer = 0;
        iter++;
    }
    int str_len = strlen(forecastToDisplay);
    iter %= str_len;
    substring[0] = forecastToDisplay[iter];
    substring[1] = forecastToDisplay[(iter + 1) % str_len];
    displayStatus(substring);
}

void start_timers()
{
    const esp_timer_create_args_t periodic_timer_args_sensor = {
        .callback = &callback_sensor,
        .name = "sensor"};
    esp_timer_handle_t periodic_timer_sensor;

    const esp_timer_create_args_t periodic_timer_args_display = {
        .callback = &callback_display,
        .name = "display"};
    esp_timer_handle_t periodic_timer_display;

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args_sensor, &periodic_timer_sensor));
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args_display, &periodic_timer_display));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_sensor, 60000000)); // 1 min
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_display, 10000)); // 10 ms
}

void app_main(void)
{
    wifi_init();
    wifi_start();
    mqtt_init();

    configure_io_ports();

    setupDisplayOnStart();

    // Configure the sensor
    CHECK(bme280_init(&busHandle, &sensorHandle, SENSOR_ADDR, SDA_PIN, SCL_PIN, CLK_SPEED_HZ));

    CHECK(bme280_default_setup(sensorHandle));

    start_timers();
}
