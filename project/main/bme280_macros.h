// Mode settings
#define MODE_SLEEP      0b00
#define MODE_FORCED     0b01
#define MODE_NORMAL     0b11

// Sample macros for the BME280 sensor
#define OVERSAMPLE_1X   0b001
#define OVERSAMPLE_2X   0b010
#define OVERSAMPLE_4X   0b011
#define OVERSAMPLE_8X   0b100
#define OVERSAMPLE_16X  0b101

// Standby time settings
#define STANDBY_0_5MS   0b000
#define STANDBY_62_5MS  0b001
#define STANDBY_125MS   0b010
#define STANDBY_250MS   0b011
#define STANDBY_500MS   0b100
#define STANDBY_1000MS  0b101
#define STANDBY_10MS    0b110
#define STANDBY_20MS    0b111

// Filter settings
#define FILTER_OFF      0b000
#define FILTER_COEFF_2  0b001
#define FILTER_COEFF_4  0b010
#define FILTER_COEFF_8  0b011
#define FILTER_COEFF_16 0b100

// SPI3W settings - this macros are overkill
#define SPI3W_OFF       0b0
#define SPI3W_ON        0b1

// Addresses
#define HUM_LSB_REG     0xFE
#define HUM_MSB_REG     0xFD
#define TEMP_XLSB_REG   0xFC
#define TEMP_LSB_REG    0xFB
#define TEMP_MSB_REG    0xFA
#define PRESS_XLSB_REG  0xF9
#define PRESS_LSB_REG   0xF8
#define PRESS_MSB_REG   0xF7
#define CONFIG_REG      0xF5
#define CTRL_MEAS_REG   0xF4
#define STATUS_REG      0xF3
#define CTRL_HUM_REG    0xF2
#define RESET_REG       0xE0
#define ID_REG          0xD0
#define CALIB_00_REG    0x88
#define CALIB_26_REG    0xE1
#define DATA_REG        0xF7