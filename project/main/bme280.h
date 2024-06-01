#ifndef __TEMP_SENSOR_TC74_H__INCLUDED__
#define __TEMP_SENSOR_TC74_H__INCLUDED__

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
 * \brief Data structure to hold BME280 sensor configuration.  
 */
typedef struct {
    uint8_t osrs_t;         /**< Temperature oversampling. */
    uint8_t osrs_p;         /**< Pressure oversampling. */
    uint8_t osrs_h;         /**< Humidity oversampling. */
    uint8_t mode;           /**< Sensor mode. */
    uint8_t t_sb;           /**< Standby time. */
    uint8_t filter;         /**< Filter coefficient. */
    uint8_t spi3w_en;       /**< SPI 3-wire enable. */
} bme280_config_t;

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
 * \brief Reads the BME280 status register to determine if the sensor is measuring.
 */
esp_err_t bme280_is_measuring(i2c_master_dev_handle_t sensorHandle, bool* isMeasuring);

/**
 * \brief Reads the BME280 status register to determine if the sensor is updating.
 */
esp_err_t bme280_is_updating(i2c_master_dev_handle_t sensorHandle, bool* isUpdating);

/**
 * \brief Sets the mode of the BME280 sensor.
 *      The mode can be one of the following:
 *         - [0] MODE_SLEEP  (default)
 *         - [1] MODE_FORCED (single measurement)
 *         - [3] MODE_NORMAL (continuous measurement)
 */
esp_err_t bme280_set_mode(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config);

/**
 * \brief Sets the oversampling rate for the BME280 sensor. 
 */
esp_err_t bme280_set_tp_oversampling(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config);

/**
 * \brief Sets the humidity oversampling rate for the BME280 sensor.  
 */
esp_err_t bme280_set_h_oversampling(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config);

/**
 * \brief Sets the standby time for the BME280 sensor.   
 */
esp_err_t bme280_set_standby_time(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config);

/**
 * \brief Sets the filter coefficient for the BME280 sensor.   
 */
esp_err_t bme280_set_filter(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config);

/**
 * \brief Sets the SPI 3-wire enable for the BME280 sensor.
 */
esp_err_t bme280_set_spi3w_en(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config);

/**
 * \brief Default setup for the BME280 sensor.
 *     The default setup is as follows:
 *        - Temperature oversampling: 1x
 *       - Pressure oversampling: 1x
 *      - Humidity oversampling: 1x
 *     - Filter coefficient: off
 */
esp_err_t bme280_default_setup(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config);

/**
 * \brief Reads the BME280 sensor data.
 */
esp_err_t bme280_read_data(i2c_master_dev_handle_t sensorHandle, bme280_data_t* data);

/**
 * \brief Data compensation for the BME280 sensor.
 */
esp_err_t bme280_compensate_data(i2c_master_dev_handle_t sensorHandle, bme280_data_t* data);

#endif // __TEMP_SENSOR_BME280_H__INCLUDED__