#include "bme280/bme280.h"
#include "esp_timer.h"
#include "wifi/wifi.h"
#include "mqtt/mqtt.h"
#include "forecast/forecast.h"
#include "bin7seg/bin7seg.h"
#include "sntp/sntp.h"
#include "spiffs/spiffs.h"
#include <string.h>

#define SPIFFS_FILE_PATH "/spiffs/data.txt"

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
        mqtt_publish(topic, aux);                               \
    } while (0)


#define setupDisplayOnStart() memcpy(forecastToDisplay, getWeatherState(-1), 7)

// Global Variables
i2c_master_bus_handle_t busHandle;      // I2C bus handle
i2c_master_dev_handle_t sensorHandle;   // I2C device handle
bme280_comp_data_t sensorData;          // Sensor data
char forecastData[40];                  // Forecast data
char forecastToDisplay[7];              // Forecast to display
FILE* f;                                // File pointer

void print_data()
{
    printf("\033[H\033[J"); // Clear the screen
    printf("\033[1;1H"); // Move cursor to line 1, column 1   
    printf("+-----------------%s-----------------+\n\
          \r| Temperature:%35.2f  *C |\n\
          \r| Pressure   :%35.2f hPa |\n\
          \r| Humidity   :%35.2f %%RH |\n\
          \r| Forecast   :%39s |\n\
          \r+-----------------------------------------------------+\n", 
            getTimestamp(),
            sensorData.temperature, sensorData.pressure, sensorData.humidity,
            forecastData
          );
    fflush(stdout);
}

void fprint_data()
{
    f = fopen(SPIFFS_FILE_PATH, "w");
    if (f == NULL) {
        ESP_LOGE("SPIFFS", "Failed to open file for writing");
        return;
    }

    // print is done this way as spiffs is not real-time with the printf
    // it is also done this way to reduce space usage
    fprintf(f, "TS:%s\n\
              \rT:%8.2f\t *C\n\
              \rP:%8.2f\thPa\n\
              \rH:%8.2f\t%%RH\n\
              \rF:%s\n", 
                getTimestamp(),
                sensorData.temperature, sensorData.pressure, sensorData.humidity,
                forecastData
           );
    fclose(f);
}

void post_data()
{
    char aux[7];
    publish(TEMP_TOPIC, sensorData.temperature);
    publish(PRESS_TOPIC, sensorData.pressure);
    publish(HUM_TOPIC, sensorData.humidity);
    mqtt_publish(FORECAST_TOPIC, forecastData);
}

void flush_data()
{
    static int iter = 0;

    if (mqtt_is_connected())
        post_data();

    else if (spiffsUsedSpace() < 90){
        if (++iter == MINUTES_BETWEEN_STORING_DATA){
            fprint_data();
            iter = 0;
            ESP_LOGI("MAIN", "SPIFFS USED %d", spiffsUsedSpace());
        }
    }
}

static void callback_sensor(void *arg)
{
    static int sensorReadIteration = 0; 
    static int forecastReady = 1;   // Flag to indicate if the forecast is ready to be computed

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
        int forecastIndex[1];
        memcpy(forecastData, computeForecast(sensorData.temperature,
                                             sensorData.pressure, 
                                             DETI_ALTITUDE, NORTH_WINDS, SUMMER,
                                             forecastIndex), 40); 
        
        memcpy(forecastToDisplay, getWeatherState(*forecastIndex), 7);
        
        forecastReady = 0;
    }

    print_data();

    flush_data();
}

static void callback_display(void *arg)
{
    static int iter = 0, timer = 0;
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
    time_init();
    init_spiffs(f, SPIFFS_FILE_PATH);

    configure_io_ports();

    setupDisplayOnStart();

    // Configure the sensor
    CHECK(bme280_init(&busHandle, &sensorHandle, SENSOR_ADDR, SDA_PIN, SCL_PIN, CLK_SPEED_HZ));

    CHECK(bme280_default_setup(sensorHandle));

    start_timers();
}
