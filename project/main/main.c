#include "bme280.h"
#include "esp_timer.h"
#include "wifi.h"
#include "mqtt.h"
#include "time.h"
#include "forecast.h"
#include <string.h>

#define SENSOR_ADDR 0x77
#define SDA_PIN 0
#define SCL_PIN 1
#define CLK_SPEED_HZ 400000

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
        mqtt_publish(topic, aux);                          \
    } while (0)

// Global Variables
i2c_master_bus_handle_t busHandle;      // I2C bus handle
i2c_master_dev_handle_t sensorHandle;   // I2C device handle
bme280_comp_data_t sensorData;          // Sensor data
int sensorReadIteration = 0;            // Sensor read iteration
int forecastReady = 1;                  // Flag to indicate if the forecast is ready to be computed
char forecastData[40];                  // Forecast data

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

    if (sensorReadIteration++ == MINUTES_BETWEEN_FORECASTS)
    {
        forecastReady = 1;
        sensorReadIteration = 0;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS); // Wait for the sensor to have the new data

    CHECK(bme280_read_data(sensorHandle, &sensorData));

    if (forecastReady)
    {
        memcpy(forecastData, computeForecast(sensorData.temperature,
                                             sensorData.pressure, 
                                             DETI_ALTITUDE, NORTH_WINDS, SUMMER), 40); 
        forecastReady = 0;
    }

    print_data();

    post_data();

    fflush(stdout);
}

void vSensorTask(void *pvParameters)
{
    while (1)
    {
        callback_sensor(NULL);


        vTaskDelay(60000 / portTICK_PERIOD_MS); // 1 min
    }
}

void vSleepTask(void *pvParameters)
{
    while (1)
    {
        esp_deep_sleep(60000000); // 1 min
    }
}

void start_timers()
{
    const esp_timer_create_args_t periodic_timer_args_sensor = {
        .callback = &callback_sensor,
        .name = "sensor"};
    esp_timer_handle_t periodic_timer_sensor;

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args_sensor, &periodic_timer_sensor));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_sensor, 60000000)); // 1 min
}

void start_tasks()
{
    TaskHandle_t xSensorHandle = NULL;
    xTaskCreate(vSensorTask, "SensorTask", 512, NULL, 5, NULL);
    configASSERT(xSensorHandle);

    TaskHandle_t xSleepHandle = NULL;
    xTaskCreate(vSleepTask, "SleepTask", 64, NULL, 3, NULL);
    configASSERT(xSleepHandle);
}

void app_main(void)
{
    wifi_init();
    wifi_start();
    mqtt_init();

    // Configure the sensor
    CHECK(bme280_init(&busHandle, &sensorHandle, SENSOR_ADDR, SDA_PIN, SCL_PIN, CLK_SPEED_HZ));

    CHECK(bme280_default_setup(sensorHandle));

    start_timers();
}
