#include "bme280.h"
#include "bme280_macros.h"

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

    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_new_master_bus(&i2cMasterCfg, pBusHandle));
    if (err != ESP_OK) return err;

    i2c_device_config_t i2cDevCfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = sensorAddr,
        .scl_speed_hz = clkSpeedHz,
    };

    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_new_device(*pBusHandle, &i2cDevCfg, pSensorHandle));
    if (err != ESP_OK) return err;
}

// Frees master bus and device handle for BME280 sensor
esp_err_t bme280_free(i2c_master_bus_handle_t busHandle,
                                         i2c_master_dev_handle_t sensorHandle)
{
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_del_master_bus(busHandle));
    if (err != ESP_OK) return err;
    
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_del_device(sensorHandle));
    if (err != ESP_OK) return err;
}

// Set the mode of the BME280 sensor
esp_err_t bme280_set_mode(i2c_master_dev_handle_t sensorHandle, uint8_t mode)
{
    uint8_t txBuf[2] = {CTRL_MEAS_REG, mode};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(sensorHandle, txBuf, 2, -1));
    if (err != ESP_OK) return err;

    return ESP_OK;
}

// Wrapper functions for setting the mode of the BME280 sensor
esp_err_t bme280_set_mode_sleep(i2c_master_dev_handle_t sensorHandle)
{
    return bme280_set_mode(sensorHandle, MODE_SLEEP);
}

esp_err_t bme280_set_mode_forced(i2c_master_dev_handle_t sensorHandle)
{
    return bme280_set_mode(sensorHandle, MODE_FORCED);
}

esp_err_t bme280_set_mode_normal(i2c_master_dev_handle_t sensorHandle)
{
    return bme280_set_mode(sensorHandle, MODE_NORMAL);
}

// Read the data from the BME280 sensor
// Note: This functions assume that the sensor is in forced mode

// Humidity
esp_err_t bme280_read_humidity(i2c_master_dev_handle_t sensorHandle, uint16_t* humidity)
{
    uint8_t rxBuf[2];
    uint8_t txBuf[1] = {HUM_MSB_REG};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit_receive(sensorHandle, txBuf, 1, rxBuf, 2, -1));
    if (err != ESP_OK) return err;

    *humidity = (rxBuf[0] << 8) | rxBuf[1];
    return ESP_OK;
}

esp_err_t bme280_read_humidity_lsb(i2c_master_dev_handle_t sensorHandle, uint8_t* humidity)
{
    uint8_t rxBuf[1];
    uint8_t txBuf[1] = {HUM_LSB_REG};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit_receive(sensorHandle, txBuf, 1, rxBuf, 1, -1));
    if (err != ESP_OK) return err;

    *humidity = rxBuf[0];
    return ESP_OK;
}

esp_err_t bme280_read_humidity_msb(i2c_master_dev_handle_t sensorHandle, uint8_t* humidity)
{
    uint8_t rxBuf[1];
    uint8_t txBuf[1] = {HUM_MSB_REG};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit_receive(sensorHandle, txBuf, 1, rxBuf, 1, -1));
    if (err != ESP_OK) return err;

    *humidity = rxBuf[0];
    return ESP_OK;
}

// Temperature
esp_err_t bme280_read_temperature(i2c_master_dev_handle_t sensorHandle, uint32_t* temperature)
{
    uint8_t rxBuf[3];
    uint8_t txBuf[1] = {TEMP_MSB_REG};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(sensorHandle, txBuf, 1, -1));
    if (err != ESP_OK) return err;

    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_receive(sensorHandle, rxBuf, 3, -1));
    if (err != ESP_OK) return err;

    *temperature = (rxBuf[0] << 12) | (rxBuf[1] << 4) | (rxBuf[2] >> 4);
    return ESP_OK;
}

// Pressure
esp_err_t bme280_read_pressure(i2c_master_dev_handle_t sensorHandle, uint32_t* pressure)
{
    uint8_t rxBuf[3];
    uint8_t txBuf[1] = {PRESS_MSB_REG};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(sensorHandle, txBuf, 1));
    if (err != ESP_OK) return err;

    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_receive(sensorHandle, rxBuf, 3));
    if (err != ESP_OK) return err;

    *pressure = (rxBuf[0] << 12) | (rxBuf[1] << 4) | (rxBuf[2] >> 4);
    return ESP_OK;
}

// Everything
esp_err_t bme280_read_data(i2c_master_dev_handle_t sensorHandle, bme280_data_t* data)
{
    err = bme280_read_humidity(sensorHandle, &data->humidity);
    if (err != ESP_OK) return err;

    err = bme280_read_temperature(sensorHandle, &data->temperature);
    if (err != ESP_OK) return err;

    err = bme280_read_pressure(sensorHandle, &data->pressure);
    if (err != ESP_OK) return err;

    return ESP_OK;
}

// Oversampling settings
esp_err_t bme280_set_temperature_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling)
{
    uint8_t txBuf[2] = {CTRL_MEAS_REG, oversampling << 5};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(sensorHandle, txBuf, 2));
    if (err != ESP_OK) return err;

    return ESP_OK;
}

esp_err_t bme280_set_pressure_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling)
{
    uint8_t txBuf[2] = {CTRL_MEAS_REG, oversampling << 2};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(sensorHandle, txBuf, 2));
    if (err != ESP_OK) return err;

    return ESP_OK;
}

esp_err_t bme280_set_humidity_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling)
{
    uint8_t txBuf[2] = {CTRL_HUM_REG, oversampling};
    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2c_master_transmit(sensorHandle, txBuf, 2));
    if (err != ESP_OK) return err;

    return ESP_OK;
}

esp_err_t bme280_set_all_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t oversampling)
{
    err = bme280_set_temperature_oversampling(sensorHandle, oversampling);
    if (err != ESP_OK) return err;

    err = bme280_set_pressure_oversampling(sensorHandle, oversampling);
    if (err != ESP_OK) return err;

    err = bme280_set_humidity_oversampling(sensorHandle, oversampling);
    if (err != ESP_OK) return err;

    return ESP_OK;
}