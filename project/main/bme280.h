#include "driver/i2c.h"
#include "driver/i2c_master.h"

/**
 * \brief Data structure to hold BME280 sensor data. 
 */
typedef struct {
    uint32_t pressure;      /**< Pressure in Pa. */
    uint32_t temperature;   /**< Temperature in 0.01 degrees Celsius. */
    uint16_t humidity;      /**< Humidity in 0.01% relative humidity. */
} bme280_data_t;

/**
 * \brief Configures master bus and device handle for BME280 sensor.
 */
esp_err_t bme280_init(i2c_master_bus_handle_t* pBusHandle,
                                        i2c_master_dev_handle_t* pSensorHandle,
                                        uint8_t sensorAddr, int sdaPin, int sclPin, uint32_t clkSpeedHz);

/**
 * \brief Frees master bus and device handle for BME280 sensor.
 */
esp_err_t bme280_free(i2c_master_bus_handle_t busHandle,
                                         i2c_master_dev_handle_t sensorHandle);


/**
 * \brief Sets the mode of the BME280 sensor.
 *      The mode can be one of the following:
 *         - [0] MODE_SLEEP  (default)
 *         - [1] MODE_FORCED (single measurement)
 *         - [3] MODE_NORMAL (continuous measurement)
 */
esp_err_t bme280_set_mode(i2c_master_dev_handle_t sensorHandle, uint8_t mode);

/**
 * \brief Sets the mode of the BME280 sensor to sleep.
 */
esp_err_t bme280_set_mode_sleep(i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Sets the mode of the BME280 sensor to forced.
 */
esp_err_t bme280_set_mode_forced(i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Sets the mode of the BME280 sensor to normal.
 */
esp_err_t bme280_set_mode_normal(i2c_master_dev_handle_t sensorHandle);


/**
 * \brief Reads the data from the BME280 sensor.
 *    Note: This function assumes that the sensor is in forced mode.
 */
esp_err_t bme280_read_data(i2c_master_dev_handle_t sensorHandle, bme280_data_t* data);

/**
 * \brief Reads the humidity from the BME280 sensor.
 */
esp_err_t bme280_read_humidity(i2c_master_dev_handle_t sensorHandle, uint16_t* humidity);

/**
 * \brief Reads the temperature from the BME280 sensor.
 */
esp_err_t bme280_read_temperature(i2c_master_dev_handle_t sensorHandle, uint32_t* temperature);

/**
 * \brief Reads the pressure from the BME280 sensor.
 */
esp_err_t bme280_read_pressure(i2c_master_dev_handle_t sensorHandle, uint32_t* pressure);

/**
 * \brief Sets the oversampling of the BME280 sensor for all measurements.
 *    The oversampling can be one of the following:
 *       - [1] OVERSAMPLE_1X
 *       - [2] OVERSAMPLE_2X
 *       - [3] OVERSAMPLE_4X
 *       - [4] OVERSAMPLE_8X
 *       - [5] OVERSAMPLE_16X
 */
esp_err_t bme280_set_all_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling);

/**
 * \brief Sets the temperature oversampling of the BME280 sensor.
 *    The oversampling can be one of the following:
 *       - [1] OVERSAMPLE_1X
 *       - [2] OVERSAMPLE_2X
 *       - [3] OVERSAMPLE_4X
 *       - [4] OVERSAMPLE_8X
 *       - [5] OVERSAMPLE_16X
 */
esp_err_t bme280_set_temperature_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling);

/**
 * \brief Sets the pressure oversampling of the BME280 sensor.
 *    The oversampling can be one of the following:
 *       - [1] OVERSAMPLE_1X
 *       - [2] OVERSAMPLE_2X
 *       - [3] OVERSAMPLE_4X
 *       - [4] OVERSAMPLE_8X
 *       - [5] OVERSAMPLE_16X
 */
esp_err_t bme280_set_pressure_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling);

/**
 * \brief Sets the humidity oversampling of the BME280 sensor.
 *    The oversampling can be one of the following:
 *       - [1] OVERSAMPLE_1X
 *       - [2] OVERSAMPLE_2X
 *       - [3] OVERSAMPLE_4X
 *       - [4] OVERSAMPLE_8X
 *       - [5] OVERSAMPLE_16X
 */
esp_err_t bme280_set_humidity_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling);

esp_err_t bme280_read_humidity_msb(i2c_master_dev_handle_t sensorHandle, uint8_t* humidity);
esp_err_t bme280_read_humidity_lsb(i2c_master_dev_handle_t sensorHandle, uint8_t* humidity);

#endif // __TEMP_SENSOR_BME280_H__INCLUDED__