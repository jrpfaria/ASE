#include "bme280.h"

// ESP Macros
#define CHECK(x) do { esp_err_t err; if ((err = ESP_ERROR_CHECK_WITHOUT_ABORT(x)) != ESP_OK) return err; } while (0)

// Global Variables
/**
 * \brief Calibration data for the BME280 sensor.
 */
static bme280_calib_data_t calibData;

/**
 * \brief Configuration data for the BME280 sensor.
 */
static bme280_config_t config;

/**
 * \brief Raw data from the BME280 sensor.
 */
static bme280_data_t rawData;


/**
 * \brief Configures master bus and device handle for BME280 sensor.
 */
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

/**
 * \brief Frees master bus and device handle for BME280 sensor.
 */
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
esp_err_t bme280_set_mode(i2c_master_dev_handle_t sensorHandle, uint8_t mode)
{
    config.mode = mode;

    uint8_t new_ctrl_meas_reg = (config.osrs_t & 0x7) << 5 | (config.osrs_p & 0x7) << 2 | (config.mode & 0x3);

    const uint8_t txBuf[2] = {CTRL_MEAS_REG, new_ctrl_meas_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf, sizeof(txBuf), -1));

    return ESP_OK;
}

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
esp_err_t bme280_set_temperature_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t osrs_t)
{
    config.osrs_t = osrs_t;

    uint8_t new_ctrl_meas_reg = (config.osrs_t & 0x7) << 5 | (config.osrs_p & 0x7) << 2 | (config.mode & 0x3);
    uint8_t txBuf[2] = {CTRL_MEAS_REG, new_ctrl_meas_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf, sizeof(txBuf), -1));

    return ESP_OK;
}

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
esp_err_t bme280_set_pressure_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t osrs_p)
{
    config.osrs_p = osrs_p;

    uint8_t new_ctrl_meas_reg = (config.osrs_t & 0x7) << 5 | (config.osrs_p & 0x7) << 2 | (config.mode & 0x3);
    uint8_t txBuf[2] = {CTRL_MEAS_REG, new_ctrl_meas_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf, sizeof(txBuf), -1));

    return ESP_OK;
}


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
esp_err_t bme280_set_humidity_oversampling(i2c_master_dev_handle_t sensorHandle, uint8_t osrs_h)
{
    config.osrs_h = osrs_h;

    // We need to start by reading the current value of the ctrl_hum register to preserve the reserved bits
    const uint8_t txBuf[1] = {CTRL_HUM_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the ctrl_hum register with the new value
    uint8_t new_ctrl_hum_reg = (rxBuf[0] & 0xF8) | (config.osrs_h & 0x7);
    uint8_t txBuf2[2] = {CTRL_HUM_REG, new_ctrl_hum_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));

    return ESP_OK;
}

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
esp_err_t bme280_set_standby_time(i2c_master_dev_handle_t sensorHandle, uint8_t t_sb)
{
    config.t_sb = t_sb;

    // We need to start by reading the current value of the config register to preserve the reserved bits
    const uint8_t txBuf[1] = {CONFIG_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the config register with the new value
    uint8_t new_config_reg = (rxBuf[0] & 0x2) | (config.t_sb & 0x7) << 5 | (config.filter & 0x7) << 2 | (config.spi3w_en & 0x1);
    uint8_t txBuf2[2] = {CONFIG_REG, new_config_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));

    return ESP_OK;
}

/**
 * \brief Sets the filter coefficient for the BME280 sensor.
 *      The filter coefficient can be one of the following:
 *         - [0] FILTER_OFF
 *         - [1] FILTER_2
 *         - [2] FILTER_4
 *         - [3] FILTER_8
 *         - [4] FILTER_16   
 */
esp_err_t bme280_set_filter(i2c_master_dev_handle_t sensorHandle, uint8_t filter)
{
    config.filter = filter;

    // We need to start by reading the current value of the config register to preserve the reserved bits
    const uint8_t txBuf[1] = {CONFIG_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the config register with the new value
    uint8_t new_config_reg = (rxBuf[0] & 0x2) | (config.t_sb & 0x7) << 5 | (config.filter & 0x7) << 2 | (config.spi3w_en & 0x1);
    uint8_t txBuf2[2] = {CONFIG_REG, new_config_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));

    return ESP_OK;
}

/**
 * \brief Sets the SPI 3-wire enable for the BME280 sensor.
 *      The SPI 3-wire enable can be one of the following:
 *         - [0] SPI3W_DISABLE
 *         - [1] SPI3W_ENABLE
 */
esp_err_t bme280_set_spi3w_en(i2c_master_dev_handle_t sensorHandle, uint8_t spi3w_en)
{
    config.spi3w_en = spi3w_en;

    // We need to start by reading the current value of the config register to preserve the reserved bits
    const uint8_t txBuf[1] = {CONFIG_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    // Update the config register with the new value
    uint8_t new_config_reg = (rxBuf[0] & 0x2) | ((config.t_sb & 0x7) << 5 | (config.filter & 0x7) << 2 | (config.spi3w_en & 0x1));
    uint8_t txBuf2[2] = {CONFIG_REG, new_config_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));
    
    return ESP_OK;
}

/**
 * \brief Default setup for the BME280 sensor.
 *      The default setup is as follows:
 *         - Temperature oversampling: 1x
 *         - Pressure oversampling: 1x
 *         - Humidity oversampling: 1x
 *         - Filter coefficient: off
 */
esp_err_t bme280_default_setup(i2c_master_dev_handle_t sensorHandle)
{
    // ctrl_meas register
    config.osrs_t = OVERSAMPLE_1X;
    config.osrs_p = OVERSAMPLE_1X;
    config.mode = MODE_SLEEP;

    const uint8_t txBuf[2] = {CTRL_MEAS_REG, (OVERSAMPLE_1X & 0x7) << 5 | (OVERSAMPLE_1X & 0x7) << 2 | (MODE_SLEEP & 0x3)};
    CHECK(i2c_master_transmit(sensorHandle, txBuf, sizeof(txBuf), -1));

    // config register
    config.t_sb = STANDBY_1000MS;
    config.filter = FILTER_OFF;
    config.spi3w_en = SPI3W_OFF;

    const uint8_t txBuf1[1] = {CONFIG_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf1, sizeof(txBuf1), rxBuf, sizeof(rxBuf), -1));

    uint8_t new_config_reg = (rxBuf[0] & 0x2) | (STANDBY_0_5MS & 0x7) << 5 | (FILTER_OFF & 0x7) << 2 | (SPI3W_OFF & 0x1);

    uint8_t txBuf2[2] = {CONFIG_REG, new_config_reg};

    CHECK(i2c_master_transmit(sensorHandle, txBuf2, sizeof(txBuf2), -1));

    // ctrl_hum register
    CHECK(bme280_set_humidity_oversampling(sensorHandle, OVERSAMPLE_1X));

    return ESP_OK;
}

/**
 * \brief Reads the BME280 calibration data.
 */
esp_err_t bme280_read_calibration_data(i2c_master_dev_handle_t sensorHandle)
{
    const uint8_t txBuf[1] = {CALIB_00_REG};
    uint8_t rxBuf[24];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    calibData.dig_T1 = (rxBuf[1] << 8) | rxBuf[0];
    calibData.dig_T2 = (rxBuf[3] << 8) | rxBuf[2];
    calibData.dig_T3 = (rxBuf[5] << 8) | rxBuf[4];
    calibData.dig_P1 = (rxBuf[7] << 8) | rxBuf[6];
    calibData.dig_P2 = (rxBuf[9] << 8) | rxBuf[8];
    calibData.dig_P3 = (rxBuf[11] << 8) | rxBuf[10];
    calibData.dig_P4 = (rxBuf[13] << 8) | rxBuf[12];
    calibData.dig_P5 = (rxBuf[15] << 8) | rxBuf[14];
    calibData.dig_P6 = (rxBuf[17] << 8) | rxBuf[16];
    calibData.dig_P7 = (rxBuf[19] << 8) | rxBuf[18];
    calibData.dig_P8 = (rxBuf[21] << 8) | rxBuf[20];
    calibData.dig_P9 = (rxBuf[23] << 8) | rxBuf[22];

    const uint8_t txBuf2[1] = {CALIB_26_REG};
    uint8_t rxBuf2[7];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf2, sizeof(txBuf2), rxBuf2, sizeof(rxBuf2), -1));

    calibData.dig_H1 = rxBuf2[0];
    calibData.dig_H2 = (rxBuf2[2] << 8) | rxBuf2[1];
    calibData.dig_H3 = rxBuf2[3];
    calibData.dig_H4 = (rxBuf2[4] << 4) | (rxBuf2[5] & 0xF);
    calibData.dig_H5 = (rxBuf2[6] << 4) | (rxBuf2[5] >> 4);
    calibData.dig_H6 = rxBuf2[7];

    return ESP_OK;
}

/**
 * \brief Temperature compensation for the BME280 sensor. 
 */
double t_fine;
double bme280_compensate_temperature(int32_t adc_T)
{
    int32_t var1, var2;

    var1 = (((double)adc_T)/16384.0 - ((double)calibData.dig_T1)/1024.0) * ((double)calibData.dig_T2);
    var2 = ((((double)adc_T)/131072.0 - ((double)calibData.dig_T1)/8192.0) *
        (((double)adc_T)/131072.0 - ((double)calibData.dig_T1)/8192.0)) * ((double)calibData.dig_T3);

    t_fine = var1 + var2;

    return (var1 + var2) / 5120.0;
}

/**
 * \brief Pressure compensation for the BME280 sensor. 
 */
double bme280_compensate_pressure(int32_t adc_P)
{
    double var1, var2, p;

    var1 = ((double)t_fine/2.0) - 64000.0;
    var2 = var1 * var1 * ((double)calibData.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)calibData.dig_P5) * 2.0;
    var2 = (var2/4.0)+(((double)calibData.dig_P4) * 65536.0);
    var1 = (((double)calibData.dig_P3) * var1 * var1 / 524288.0 + 
        ((double)calibData.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)calibData.dig_P1);
    if (var1 == 0.0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)calibData.dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)calibData.dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)calibData.dig_P7)) / 16.0;
    return p;
}

/**
 * \brief Humidity compensation for the BME280 sensor. 
 */
double bme280_compensate_humidity(int32_t adc_H)
{
    double var_H;

    var_H = (t_fine - 76800.0);
    var_H = (adc_H - (((double)calibData.dig_H4) * 64.0 + ((double)calibData.dig_H5) / 16384.0 * var_H)) *
        (((double)calibData.dig_H2) / 65536.0 * (1.0 + ((double)calibData.dig_H6) / 67108864.0 * var_H *
        (1.0 + ((double)calibData.dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)calibData.dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    return var_H;
}

/**
 * \brief Data compensation for the BME280 sensor.
 */
void bme280_compensate_data(bme280_comp_data_t* data)
{
    data->temperature = bme280_compensate_temperature(rawData.temperature);
    data->pressure = bme280_compensate_pressure(rawData.pressure);
    data->humidity = bme280_compensate_humidity(rawData.humidity);
}

/**
 * \brief Reads the BME280 sensor data.
 */
esp_err_t bme280_read_data(i2c_master_dev_handle_t sensorHandle, bme280_data_t* rData, bme280_comp_data_t* data)
{
    const uint8_t txBuf[1] = {DATA_REG};
    uint8_t rxBuf[8];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    rawData.pressure = (rxBuf[0] << 12) | (rxBuf[1] << 4) | (rxBuf[2] >> 4);
    rawData.temperature = (rxBuf[3] << 12) | (rxBuf[4] << 4) | (rxBuf[5] >> 4);
    rawData.humidity = (rxBuf[6] << 8) | rxBuf[7];

    rData->pressure = rawData.pressure;
    rData->temperature = rawData.temperature;
    rData->humidity = rawData.humidity;

    bme280_compensate_data(data);

    return ESP_OK;
}

/**
 * \brief Reads the BME280 sensor id. 
 *     This is a sanity check to ensure the sensor is connected.
 */
esp_err_t bme280_read_id(i2c_master_dev_handle_t sensorHandle, uint8_t* id)
{
    const uint8_t txBuf[1] = {ID_REG};

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), id, sizeof(id), -1));

    return ESP_OK;
}

/**
 * \brief Reads the BME280 mode. 
 */
esp_err_t bme280_read_mode(i2c_master_dev_handle_t sensorHandle, uint8_t* mode)
{
    const uint8_t txBuf[1] = {CTRL_MEAS_REG};
    uint8_t rxBuf[1];

    CHECK(i2c_master_transmit_receive(sensorHandle, txBuf, sizeof(txBuf), rxBuf, sizeof(rxBuf), -1));

    *mode = rxBuf[0] & 0x3;

    return ESP_OK;
}