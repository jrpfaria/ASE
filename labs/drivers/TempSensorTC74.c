#include "TempSensorTC74.h"

// ESP Macros
#define check(x) esp_err_t ret = ESP_ERROR_CHECK_WITHOUT_ABORT(x); if(ret != ESP_OK) return ret;
#define ok return ESP_OK;
#define tx i2c_master_transmit
#define rx i2c_master_receive
#define txrx i2c_master_transmit_receive

esp_err_t tc74_init(i2c_master_bus_handle_t* pBusHandle,
					i2c_master_dev_handle_t* pSensorHandle,
					uint8_t sensorAddr, int sdaPin, int sclPin, uint32_t clkSpeedHz)
{
    i2c_master_bus_config_t i2cMasterCfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = sclPin,
        .sda_io_num = sdaPin,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    
    check(i2c_new_master_bus(&i2cMasterCfg, pBusHandle))

    i2c_device_config_t i2cDevCfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = sensorAddr,
        .scl_speed_hz = clkSpeedHz,
    };

    check(i2c_master_bus_add_device(*pBusHandle, &i2cDevCfg, pSensorHandle))

    ok
}

esp_err_t tc74_free(i2c_master_bus_handle_t busHandle,
					 i2c_master_dev_handle_t sensorHandle)
{
    check(i2c_del_master_bus(busHandle))
    check(i2c_master_bus_rm_device(sensorHandle))

    ok
}

esp_err_t tc74_standy(i2c_master_dev_handle_t sensorHandle)
{
    const uint8_t txBuf[2] = {0x01, 0x80};
    
    check( tx (sensorHandle, txBuf, sizeof(txBuf), -1))

    ok
}

esp_err_t tc74_wakeup(i2c_master_dev_handle_t sensorHandle)
{
    const uint8_t txBuf[2] = {0x01, 0x00};

    check( tx (sensorHandle, txBuf, sizeof(txBuf), -1))

    ok
}

bool tc74_is_temperature_ready(i2c_master_dev_handle_t sensorHandle)
{
    const uint8_t txBuf = 0x01;
    uint8_t rxBuf;
    
    check( txrx (sensorHandle, &txBuf, sizeof(txBuf), &rxBuf, sizeof(rxBuf), -1))
    
    return (rxBuf & 0x40);    
}

esp_err_t tc74_wakeup_and_read_temp(i2c_master_dev_handle_t sensorHandle, uint8_t* pTemp)
{
    check(tc74_wakeup(sensorHandle))

    check(tc74_read_temp_after_cfg(sensorHandle, pTemp))

    ok
}

esp_err_t tc74_read_temp_after_cfg(i2c_master_dev_handle_t sensorHandle, uint8_t* pTemp)
{
    const uint8_t txBuf = 0x00;
    check( txrx (sensorHandle, &txBuf, sizeof(txBuf), pTemp, sizeof(pTemp), -1))

    ok
}

esp_err_t tc74_read_temp_after_temp(i2c_master_dev_handle_t sensorHandle, uint8_t* pTemp)
{
    check( rx (sensorHandle, pTemp, sizeof(pTemp), -1))

    ok
}