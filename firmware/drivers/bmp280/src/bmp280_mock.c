// bsp/src/pc/bmp280_bsp_mock.c
#include "bmp280_mock.h"
#include "bmp280_definitions.h"
#include "udp.h"
#include <string.h>
#include <math.h>
#include <windows.h>
#include <stdio.h>

// =========================================================
// Little-endian, registers 0x88 to 0x9F
// T1=27504, T2=26435, T3=-1000
// P1=36477, P2=-10685, P3=3024, P4=2855, P5=140
// P6=-7, P7=15500, P8=-14600, P9=6000
// =========================================================
static const uint8_t calib_bytes[24] = {
    0x90, 0x6B,   // T1 = 27504
    0x43, 0x67,   // T2 = 26435
    0x18, 0xFC,   // T3 = -1000
    0x7D, 0x8E,   // P1 = 36477
    0x43, 0xD6,   // P2 = -10685
    0xD0, 0x0B,   // P3 =  3024
    0x27, 0x0B,   // P4 =  2855
    0x8C, 0x00,   // P5 =  140
    0xF9, 0xFF,   // P6 = -7
    0x8C, 0x3C,   // P7 =  15500
    0xF8, 0xC6,   // P8 = -14600
    0x70, 0x17    // P9 =  6000
};

// =========================================================
// Mock function implementations
// =========================================================

static int mock_init(uint8_t deviceAddress) {
    (void)deviceAddress;
    printf("[BMP280] Initializing UDP\n");
    return udp_io_init("0.0.0.0", 5001, "127.0.0.1", 5002);
}

static int mock_deinit(uint8_t deviceAddress) {
    (void)deviceAddress;
    udp_io_close();
    return 0;
}

static int mock_delay(uint32_t ms) {
    Sleep(ms);
    return 0;
}

static int mock_pow(float x, float y, float *result) {
    *result = powf(x, y);
    return 0;
}

static int mock_write(uint8_t deviceAddress, uint8_t reg,
                      uint8_t *data, uint8_t len) {
    // No-op: sensor config is set directly in Simulink
    (void)deviceAddress; (void)reg; (void)data; (void)len;
    return 0;
}

static BmpSimPacket_t cached_pkt = {0};
static int pkt_valid = 0;

static int mock_read(uint8_t deviceAddress, uint8_t reg,
                     uint8_t *data, uint8_t len) {
    (void)deviceAddress;

    if (reg == BMP280_REGISTER_ADDRESS_ID) {
        data[0] = 0x58;
        return 0;
    }

    if (reg >= 0x88 && reg <= 0x9F) {
        memcpy(data, calib_bytes + (reg - 0x88), len);
        return 0;
    }

    if (reg == BMP280_REGISTER_ADDRESS_STATUS) {
        data[0] = 0x00;
        return 0;
    }

    // --- Temperature read (0xFA) — driver reads this FIRST ---
    if (reg == BMP280_REGISTER_ADDRESS_TEMPERATURE_MSB) {
        if (udp_io_receive_timeout(&cached_pkt, sizeof(cached_pkt), 100) != 0) {
            fprintf(stderr, "[BMP280 MOCK] I2C timeout\n");
            pkt_valid = 0;
            return -1;
        }
        pkt_valid = 1;

        data[0] = (uint8_t)((cached_pkt.adc_T >> 12) & 0xFF);
        data[1] = (uint8_t)((cached_pkt.adc_T >>  4) & 0xFF);
        data[2] = (uint8_t)((cached_pkt.adc_T & 0x0F) << 4);
        return 0;
    }

    // --- Pressure read (0xF7) — reuses cached packet from temperature read ---
    if (reg == BMP280_REGISTER_ADDRESS_PRESSURE_MSB) {
        if (!pkt_valid) {
            // fallback: fetch if temperature wasn't read first
            if (udp_io_receive_timeout(&cached_pkt, sizeof(cached_pkt), 100) != 0) {
                fprintf(stderr, "[BMP280 MOCK] I2C timeout\n");
                return -1;
            }
        }

        data[0] = (uint8_t)((cached_pkt.adc_P >> 12) & 0xFF);
        data[1] = (uint8_t)((cached_pkt.adc_P >>  4) & 0xFF);
        data[2] = (uint8_t)((cached_pkt.adc_P & 0x0F) << 4);

        // burst read (len=6): fill temperature bytes too
        if (len >= 6) {
            data[3] = (uint8_t)((cached_pkt.adc_T >> 12) & 0xFF);
            data[4] = (uint8_t)((cached_pkt.adc_T >>  4) & 0xFF);
            data[5] = (uint8_t)((cached_pkt.adc_T & 0x0F) << 4);
        }

        pkt_valid = 0;  // consumed — next cycle fetches fresh data
        return 0;
    }

    return 0;
}

// Mutex no-ops for PC 
static int mock_lock(void *handle)   { (void)handle; return 0; }
static int mock_unlock(void *handle) { (void)handle; return 0; }

static int dummy_mutex = 0;   // ← add this

void bmp280_bsp_mock_init_dependency(bmp280_dependency_t *dep) {
    dep->bmp280_interface_init    = mock_init;
    dep->bmp280_interface_deinit  = mock_deinit;
    dep->bmp280_delay_function    = mock_delay;
    dep->bmp280_write_array       = mock_write;
    dep->bmp280_read_array        = mock_read;
    dep->bmp280_power_function    = mock_pow;

    dep->bmp280_interface_exclusion.bmp280_interface_lock   = mock_lock;
    dep->bmp280_interface_exclusion.bmp280_interface_unlock = mock_unlock;
    dep->bmp280_interface_exclusion.mutex_handle            = &dummy_mutex;
}