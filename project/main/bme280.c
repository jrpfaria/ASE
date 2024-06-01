#include "bme280.h"
#include "bme280_macros.h"

// ESP Macros
#define CHECK(x) do { esp_err_t err; if ((err = ESP_ERROR_CHECK_WITHOUT_ABORT(x)) != ESP_OK) return err; } while (0)

esp_err_t err;

// Configures master bus and device handle for BME280 sensor
esp_err_t bme280_init(i2c_master_bus_handle_t* pBusHandle,
                                        i2c_master_dev_handle_t* pSensorHandle,
                                        uint8_t sensorAddr, int sdaPin, int sclPin, uint32_t clkSpeedHz)
{
    i2c_master_bus_config_t i2cMasterCfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port   = I2C_NUM_0,
        .sda_io_num = sdaPin,
        .scl_io_num = sclPin,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    CHECK(i2c_new_master_bus(&i2cMasterCfg, pBusHandle));

    i2c_device_config_t i2cDevCfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = sensorAddr,
        .scl_speed_hz = clkSpeedHz,
    };

    CHECK(i2c_master_bus_add_device(*pBusHandle, &i2cDevCfg, pSensorHandle));

    return ESP_OK;
}

// Frees master bus and device handle for BME280 sensor
esp_err_t bme280_free(i2c_master_bus_handle_t busHandle,
                                         i2c_master_dev_handle_t sensorHandle)
{
    CHECK(i2c_del_master_bus(busHandle));

    CHECK(i2c_master_bus_rm_device(sensorHandle));

    return ESP_OK;
}

/**
 * \brief Reads the BME280 status register to determine if the sensor is measuring.
 */
esp_err_t bme280_is_measuring(i2c_master_dev_handle_t sensorHandle, bool* isMeasuring)
{
    const uint8_t txBuf[1] = {STATUS_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    *isMeasuring = (rxBuf[0] & 0x8) >> 3;

    return ESP_OK;
}

/**
 * \brief Reads the BME280 status register to determine if the sensor is updating.
 */
esp_err_t bme280_is_updating(i2c_master_dev_handle_t sensorHandle, bool* isUpdating)
{
    const uint8_t txBuf[1] = {STATUS_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    *isUpdating = (rxBuf[0] & 0x1);

    return ESP_OK;
}

/**
 * \brief Sets the mode of the BME280 sensor.
 *      The mode can be one of the following:
 *         - [0] MODE_SLEEP  (default)
 *         - [1] MODE_FORCED (single measurement)
 *         - [3] MODE_NORMAL (continuous measurement)
 */
esp_err_t bme280_set_mode(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config)
{
    uint8_t new_ctrl_meas_reg = (config->osrs_t & 0x7) << 5 | (config->osrs_p & 0x7) << 2 | (config->mode & 0x3);

    const uint8_t txBuf[2] = {CTRL_MEAS_REG, new_ctrl_meas_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf, sizeof(txBuf), -1));

    return ESP_OK;
}

/**
 * \brief Sets the oversampling rate for the BME280 sensor. 
 */
esp_err_t bme280_set_tp_oversampling(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config)
{
    uint8_t new_ctrl_meas_reg = (config->osrs_t & 0x7) << 5 | (config->osrs_p & 0x7) << 2 | (config->mode & 0x3);
    uint8_t txBuf[2] = {CTRL_MEAS_REG, new_ctrl_meas_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf, sizeof(txBuf), -1));

    return ESP_OK;
}

/**
 * \brief Sets the humidity oversampling rate for the BME280 sensor.  
 */
esp_err_t bme280_set_h_oversampling(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config)
{
    // We need to start by reading the current value of the ctrl_hum register to preserve the reserved bits
    const uint8_t txBuf[1] = {CTRL_HUM_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the ctrl_hum register with the new value
    uint8_t new_ctrl_hum_reg = (rxBuf[0] & 0xF8) | (config->osrs_h & 0x7);
    uint8_t txBuf2[2] = {CTRL_HUM_REG, new_ctrl_hum_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));

    return ESP_OK;
}

/**
 * \brief Sets the standby time for the BME280 sensor.   
 */
esp_err_t bme280_set_standby_time(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config)
{
    // We need to start by reading the current value of the config register to preserve the reserved bits
    const uint8_t txBuf[1] = {CONFIG_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the config register with the new value
    uint8_t new_config_reg = (rxBuf[0] & 0xE7) | (config->t_sb & 0x7) << 5 | (config->filter & 0x7) << 2 | (config->spi3w_en & 0x1);
    uint8_t txBuf2[2] = {CONFIG_REG, new_config_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));

    return ESP_OK;
}

/**
 * \brief Sets the filter coefficient for the BME280 sensor.   
 */
esp_err_t bme280_set_filter(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config)
{
    // We need to start by reading the current value of the config register to preserve the reserved bits
    const uint8_t txBuf[1] = {CONFIG_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the config register with the new value
    uint8_t new_config_reg = (rxBuf[0] & 0xE7) | (config->t_sb & 0x7) << 5 | (config->filter & 0x7) << 2 | (config->spi3w_en & 0x1);
    uint8_t txBuf2[2] = {CONFIG_REG, new_config_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));

    return ESP_OK;
}

/**
 * \brief Sets the SPI 3-wire enable for the BME280 sensor.
 */
esp_err_t bme280_set_spi3w_en(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config)
{
    // We need to start by reading the current value of the config register to preserve the reserved bits
    const uint8_t txBuf[1] = {CONFIG_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the config register with the new value
    uint8_t new_config_reg = (rxBuf[0] & 0xE7) | (config->t_sb & 0x7) << 5 | (config->filter & 0x7) << 2 | (config->spi3w_en & 0x1);
    uint8_t txBuf2[2] = {CONFIG_REG, new_config_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));
    
    return ESP_OK;
}

/**
 * \brief Default setup for the BME280 sensor.
 *     The default setup is as follows:
 *        - Temperature oversampling: 1x
 *       - Pressure oversampling: 1x
 *      - Humidity oversampling: 1x
 *     - Filter coefficient: off
 */
esp_err_t bme280_default_setup(i2c_master_dev_handle_t sensorHandle, bme280_config_t* config)
{

    return ESP_OK;
}

/**
 * \brief Reads the BME280 calibration data.
 */
esp_err_t bme280_read_calibration_data(i2c_master_dev_handle_t sensorHandle, bme280_calib_data_t* calibData)
{
    const uint8_t txBuf[1] = {CALIB_00_REG};
    uint8_t rxBuf[24];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    calibData->dig_T1 = (rxBuf[1] << 8) | rxBuf[0];
    calibData->dig_T2 = (rxBuf[3] << 8) | rxBuf[2];
    calibData->dig_T3 = (rxBuf[5] << 8) | rxBuf[4];
    calibData->dig_P1 = (rxBuf[7] << 8) | rxBuf[6];
    calibData->dig_P2 = (rxBuf[9] << 8) | rxBuf[8];
    calibData->dig_P3 = (rxBuf[11] << 8) | rxBuf[10];
    calibData->dig_P4 = (rxBuf[13] << 8) | rxBuf[12];
    calibData->dig_P5 = (rxBuf[15] << 8) | rxBuf[14];
    calibData->dig_P6 = (rxBuf[17] << 8) | rxBuf[16];
    calibData->dig_P7 = (rxBuf[19] << 8) | rxBuf[18];
    calibData->dig_P8 = (rxBuf[21] << 8) | rxBuf[20];
    calibData->dig_P9 = (rxBuf[23] << 8) | rxBuf[22];

    const uint8_t txBuf2[1] = {CALIB_26_REG};
    uint8_t rxBuf2[7];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf2, sizeof(txBuf2), rxBuf2, sizeof(rxBuf2), -1));

    calibData->dig_H1 = rxBuf2[0];
    calibData->dig_H2 = (rxBuf2[2] << 8) | rxBuf2[1];
    calibData->dig_H3 = rxBuf2[3];
    calibData->dig_H4 = (rxBuf2[4] << 4) | (rxBuf2[5] & 0xF);
    calibData->dig_H5 = (rxBuf2[6] << 4) | (rxBuf2[5] >> 4);
    calibData->dig_H6 = rxBuf2[7];

    return ESP_OK;
}

/**
 * \brief Temperature compensation for the BME280 sensor. 
 */
double bme280_compensate_temperature(int32_t adc_T)
{

    return 0.0;
}

/**
 * \brief Pressure compensation for the BME280 sensor. 
 */
double bme280_compensate_pressure(int32_t adc_P)
{

    return 0.0;
}

/**
 * \brief Humidity compensation for the BME280 sensor. 
 */
double bme280_compensate_humidity(int32_t adc_H)
{

    return 0.0;
}


/**
 * \brief Data compensation for the BME280 sensor.
 */
bme280_comp_data_t bme280_compensate_data(bme280_data_t* data)
{
    bme280_comp_data_t compData;

    compData.temperature = bme280_compensate_temperature(data->temperature);
    compData.pressure = bme280_compensate_pressure(data->pressure);
    compData.humidity = bme280_compensate_humidity(data->humidity);

    return 
}

/**
 * \brief Reads the BME280 sensor data.
 */
esp_err_t bme280_read_data(i2c_master_dev_handle_t sensorHandle, bme280_data_t* data)
{
    const uint8_t txBuf[1] = {DATA_REG};
    uint8_t rxBuf[8];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    data->pressure = (rxBuf[0] << 12) | (rxBuf[1] << 4) | (rxBuf[2] >> 4);
    data->temperature = (rxBuf[3] << 12) | (rxBuf[4] << 4) | (rxBuf[5] >> 4);
    data->humidity = (rxBuf[6] << 8) | rxBuf[7];

    bme280_compensate_data(data);

    return ESP_OK;
}