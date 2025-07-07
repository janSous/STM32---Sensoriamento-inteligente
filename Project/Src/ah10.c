#include "aht10.h"

OPERATION_STATE aht_Write(uint8_t cmd) {
    return AHT10_OK;
}
OPERATION_STATE aht_Read(uint8_t cmd, uint8_t* buffer, uint8_t length) {
    return AHT10_OK; 
}

uint32_t getTemperature(void) {
    uint8_t buffer[6];
    if (aht_Read(AHT_CMD_TRIGGER_MEASURE, buffer, sizeof(buffer)) == AHT10_OK) {
        return 0;
    }
    return 0;
}

uint32_t getHumidity(void) {
    uint8_t buffer[6];
    if (aht_Read(AHT_CMD_TRIGGER_MEASURE, buffer, sizeof(buffer)) == AHT10_OK) {
        return 0; 
    }
    return 0;
}
