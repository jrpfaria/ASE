#ifndef __TEMP_SENSOR_TC74_H__INCLUDED__
#define __TEMP_SENSOR_TC74_H__INCLUDED__

#include "driver/i2c.h"
#include "driver/i2c_master.h"

/**
 * \brief Data structure to hold raw BME280 sensor data. 
 */
typedef struct {
    int32_t pressure;      /**< Pressure in Pa. */
    int32_t temperature;   /**< Temperature in 0.01 degrees Celsius. */
    int32_t humidity;      /**< Humidity in 0.01% relative humidity. */
} bme280_data_t;

/**
 * \brief Data structure to hold compensated BME280 sensor data. 
 */
typedef struct {
    double temperature;    /**< Temperature in 0.01 degrees Celsius. */
    double pressure;      /**< Pressure in Pa. */
    double humidity;      /**< Humidity in 0.01% relative humidity. */
} bme280_comp_data_t;

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
 * \brief Data sturcture to hold BME280 sensor calibration data. 
 */
typedef struct {
    uint16_t dig_T1;        /**< Temperature calibration data. */
    int16_t dig_T2;         /**< Temperature calibration data. */
    int16_t dig_T3;         /**< Temperature calibration data. */
    uint16_t dig_P1;        /**< Pressure calibration data. */
    int16_t dig_P2;         /**< Pressure calibration data. */
    int16_t dig_P3;         /**< Pressure calibration data. */
    int16_t dig_P4;         /**< Pressure calibration data. */
    int16_t dig_P5;         /**< Pressure calibration data. */
    int16_t dig_P6;         /**< Pressure calibration data. */
    int16_t dig_P7;         /**< Pressure calibration data. */
    int16_t dig_P8;         /**< Pressure calibration data. */
    int16_t dig_P9;         /**< Pressure calibration data. */
    uint8_t dig_H1;         /**< Humidity calibration data. */
    int16_t dig_H2;         /**< Humidity calibration data. */
    uint8_t dig_H3;         /**< Humidity calibration data. */
    int16_t dig_H4;         /**< Humidity calibration data. */
    int16_t dig_H5;         /**< Humidity calibration data. */
    int8_t dig_H6;          /**< Humidity calibration data. */
} bme280_calib_data_t;

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
esp_err_t bme280_set_mode(i2c_master_dev_handle_t sensorHandle, uint8_t mode);

/**
 * \brief Sets the temperature oversampling rate for the BME280 sensor. 
 *      The oversampling rate can be one of the following:
 *         - [0] OVERSAMPLING_SKIP (output set to 0x80000)
 *         - [1] OVERSAMPLING_1X
 *         - [2] OVERSAMPLING_2X
 *         - [3] OVERSAMPLING_4X
 *         - [4] OVERSAMPLING_8X
 *         - [5] OVERSAMPLING_16X
 */
esp_err_t bme280_set_temperature_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t osrs_t);

/**
 * \brief Sets the pressure oversampling rate for the BME280 sensor. 
 *      The oversampling rate can be one of the following:
 *         - [0] OVERSAMPLING_SKIP (output set to 0x80000)
 *         - [1] OVERSAMPLING_1X
 *         - [2] OVERSAMPLING_2X
 *         - [3] OVERSAMPLING_4X
 *         - [4] OVERSAMPLING_8X
 *         - [5] OVERSAMPLING_16X
 */
esp_err_t bme280_set_pressure_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t osrs_t);


/**
 * \brief Sets the humidity oversampling rate for the BME280 sensor.
 *      The humidity oversampling rate can be one of the following:
 *         - [0] OVERSAMPLING_SKIP (output set to 0x80000)
 *         - [1] OVERSAMPLING_1X
 *         - [2] OVERSAMPLING_2X
 *         - [3] OVERSAMPLING_4X
 *         - [4] OVERSAMPLING_8X
 *         - [5] OVERSAMPLING_16X  
 */
esp_err_t bme280_set_humidity_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t osrs_h);

/**
 * \brief Sets the standby time for the BME280 sensor.  
 *      The standby time can be one of the following:
 *         - [0] STANDBY_0_5MS
 *         - [1] STANDBY_62_5MS
 *         - [2] STANDBY_125MS
 *         - [3] STANDBY_250MS
 *         - [4] STANDBY_500MS
 *         - [5] STANDBY_1000MS 
 */
esp_err_t bme280_set_standby_time(i2c_master_dev_handle_t sensorHandle, uint8_t t_sb);

/**
 * \brief Sets the filter coefficient for the BME280 sensor.
 *      The filter coefficient can be one of the following:
 *         - [0] FILTER_OFF
 *         - [1] FILTER_2
 *         - [2] FILTER_4
 *         - [3] FILTER_8
 *         - [4] FILTER_16   
 */
esp_err_t bme280_set_filter(i2c_master_dev_handle_t sensorHandle, uint8_t filter);

/**
 * \brief Sets the SPI 3-wire enable for the BME280 sensor.
 *      The SPI 3-wire enable can be one of the following:
 *         - [0] SPI3W_DISABLE
 *         - [1] SPI3W_ENABLE
 */
esp_err_t bme280_set_spi3w_en(i2c_master_dev_handle_t sensorHandle, uint8_t spi3w_en);

/**
 * \brief Default setup for the BME280 sensor.
 *      The default setup is as follows:
 *         - Temperature oversampling: 1x
 *         - Pressure oversampling: 1x
 *         - Humidity oversampling: 1x
 *         - Filter coefficient: off
 */
esp_err_t bme280_default_setup(i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Reads the BME280 calibration data.
 */
esp_err_t bme280_read_calibration_data(i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Temperature compensation for the BME280 sensor. 
 */
double bme280_compensate_temperature(int32_t adc_T);

/**
 * \brief Pressure compensation for the BME280 sensor. 
 */
double bme280_compensate_pressure(int32_t adc_P);

/**
 * \brief Humidity compensation for the BME280 sensor. 
 */
double bme280_compensate_humidity(int32_t adc_H);

/**
 * \brief Data compensation for the BME280 sensor.
 */
void bme280_compensate_data(bme280_data_t* rawData, bme280_comp_data_t* data);

/**
 * \brief Reads the BME280 sensor data.
 */
esp_err_t bme280_read_data(i2c_master_dev_handle_t sensorHandle, bme280_comp_data_t* data);

#endif // __TEMP_SENSOR_BME280_H__INCLUDED__