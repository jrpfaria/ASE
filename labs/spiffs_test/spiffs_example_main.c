#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "TempSensorTC74.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_spiffs.h"

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
    0b01000000
};

void configure_io_ports(void) {
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

void display(char value) {   
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

void display_b10(int value) {   
    static char active_display = 0;
    char digit;
    if (value>=0) digit = active_display ? (value / 10) : (value % 10);
    else digit = active_display ? 16 : (-value % 10);
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

int8_t temperature[1]; // Temperature value
short int timer = 0; // Timer value
FILE* f;

static void print_from_file(void){
    f = fopen("/spiffs/temperature.txt", "r");

    // Allocate memory for a line buffer
    char line[64];

    ESP_LOGI("SPIFFS", "Reading file");
    // Read each line and print it
    for (int i = 0; i < 10; i++){
        fgets(line, 64, f);
        char* pos = strchr(line, '\n');
        if (pos) {
            *pos = '\0';
        }
        ESP_LOGI("SPIFFS", "Temperature: %s", line);
    }
    
    ESP_LOGI("SPIFFS", "End of file");

    // Close the file
    fclose(f);
}

int limit = 10;
int count = 0;
static void callback_sensor(void* arg) {
    // Read the temperature
    tc74_read_temp_after_temp(sensorHandle, temperature);
    if (count++ < limit) {
        // Print to spiffs
        fprintf(f, "%d\n", temperature[0]);
        ESP_LOGI("SPIFFS", "Temperature written to file");  
    }
    if (count == limit) {
        fclose(f);
        ESP_LOGI("SPIFFS", "File closed");
        print_from_file();
        esp_vfs_spiffs_unregister(NULL);
        ESP_LOGI("SPIFFS", "SPIFFS unmounted");
    }
    printf("\r%d", temperature[0]);
    fflush(stdout);
}

static void callback_display(void* arg) {
    // Display the temperature value on the 7-segment display
    display_b10(temperature[0]);
}

void start_timers() {
    const esp_timer_create_args_t periodic_timer_args_display = {
            .callback = &callback_display,
            .name = "periodic"
    };
    const esp_timer_create_args_t periodic_timer_args_sensor = {
            .callback = &callback_sensor,
            .name = "periodic"
    };
    esp_timer_handle_t periodic_timer_display;
    esp_timer_handle_t periodic_timer_sensor;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args_display, &periodic_timer_display));
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args_sensor, &periodic_timer_sensor));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_display, 10000)); // 10ms
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer_sensor, 1000000)); // 1s
}

void init_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("SPIFFS", "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE("SPIFFS", "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE("SPIFFS", "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

#ifdef CONFIG_EXAMPLE_SPIFFS_CHECK_ON_START
    ESP_LOGI("SPIFFS", "Performing SPIFFS_check().");
    ret = esp_spiffs_check(conf.partition_label);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIFFS", "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
        return;
    } else {
        ESP_LOGI("SPIFFS", "SPIFFS_check() successful");
    }
#endif

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE("SPIFFS", "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI("SPIFFS", "Partition size: total: %d, used: %d", total, used);
    }

    if (used > total) {
        ESP_LOGW("SPIFFS", "Number of used bytes cannot be larger than total. Performing SPIFFS_check().");
        ret = esp_spiffs_check(conf.partition_label);
        if (ret != ESP_OK) {
            ESP_LOGE("SPIFFS", "SPIFFS_check() failed (%s)", esp_err_to_name(ret));
            return;
        }
        else {
            ESP_LOGI("SPIFFS", "SPIFFS_check() successful");
        }
    }
}

void app_main(void) {
    // Configure the I/O ports
    configure_io_ports();
    // Initialize the SPIFFS
    init_spiffs();
    f = fopen("/spiffs/temperature.txt", "w");
    if (f == NULL) {
        ESP_LOGE("SPIFFS", "Failed to open file for writing");
        return;
    }
    // Configure the sensor
    uint8_t sensorAddr = 0x49;
    int sdaPin = SDA;
    int sclPin = SCL;
    uint32_t clkSpeedHz = 50000;
    tc74_init(&busHandle, &sensorHandle, sensorAddr, sdaPin, sclPin, clkSpeedHz);
    tc74_wakeup_and_read_temp(sensorHandle, temperature);

    start_timers();
}
