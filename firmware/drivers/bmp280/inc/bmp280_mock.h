#ifndef BMP280_MOCK_H
#define BMP280_MOCK_H

#include "bmp280.h"
#include <stdint.h>

// Packet sent by Simulink: two int32 values, 8 bytes total
typedef struct {
    int32_t adc_P;
    int32_t adc_T;
} BmpSimPacket_t;

// Fills the bmp280_dependency_t with PC mock functions
void bmp280_bsp_mock_init_dependency(bmp280_dependency_t *dep);

#endif