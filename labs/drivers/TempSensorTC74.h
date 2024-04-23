#ifndef __TEMP_SENSOR_TC74_H__INCLUDED__
#define __TEMP_SENSOR_TC74_H__INCLUDED__

#include "driver/i2c_master.h"

/**
 * \brief Configures master bus and device handle for TC74 sensor.
 */
esp_err_t tc74_init(i2c_master_bus_handle_t* pBusHandle,
					i2c_master_dev_handle_t* pSensorHandle,
					uint8_t sensorAddr, int sdaPin, int sclPin, uint32_t clkSpeedHz);

/**
 * \brief Frees master bus and device handle for TC74 sensor. 
 */
esp_err_t tc_74_free(i2c_master_bus_handle_t busHandle,
					 i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Write standby mode to TC74 sensor.
 * Changes the configuration register (0x01) to standby mode (D[7] = 1).
 */
esp_err_t tc74_standy(i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Wrirte normal mode to TC74 sensor.
 * Changes the configuration register (0x01) to standby mode (D[7] = 0).
 */
esp_err_t tc74_wakeup(i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Check if temperature is ready to be read.
 * Checks the configuration register (0x01) to see if the temperature is ready to be read (D[6] = 1).
 */
bool tc74_is_temperature_ready(i2c_master_dev_handle_t sensorHandle);

/**
 * \brief Wakeup sensor and read temperature.
 * This function is a combination of tc74_wakeup and tc74_read_temp_after_cfg.
 */
esp_err_t tc74_wakeup_and_read_temp(i2c_master_dev_handle_t sensorHandle, uint8_t* pTemp);

/**
 * \brief Read temperature assuming last action was configuration.
 * This function reads the temperature from the sensor after the configuration was set.
 */
esp_err_t tc74_read_temp_after_cfg(i2c_master_dev_handle_t sensorHandle, uint8_t* pTemp);

/**
 * \brief Read temperature assuming last action was temperature read.
 * This function simply reads the temperature from the sensor.
 */
esp_err_t tc74_read_temp_after_temp(i2c_master_dev_handle_t sensorHandle, uint8_t* pTemp);

#endif // __TEMP_SENSOR_TC74_H__INCLUDED__
