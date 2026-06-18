#include "bmp280_driver.h"
#include "bmp280.h"           // ebrezadev lib

// BSP selected at compile time via -DPC flag
#ifdef DEBUG_PC
    #include "bmp280_mock.h"
    #define bmp280_bsp_init_dep(dep) bmp280_bsp_mock_init_dependency(dep)
#else
    #include "bmp280_bsp_i2c.h"
    #define bmp280_bsp_init_dep(dep) bmp280_bsp_i2c_init_dependency(dep)
#endif

static bmp280_handle_t handle;

int bmp280_driver_init(void) {
    // 1. Wire the correct BSP into the driver handle
    bmp280_bsp_init_dep(&handle.dependency_interface);

    if (bmp280_init(&handle, BMP280_I2C, BMP280_I2C_ADDRESS_1) != BMP280_ERROR_OK)
        return -1;

    bmp280_set_pressure_oversampling   (&handle, BMP280_OVERSAMPLING_16X);
    bmp280_set_temperature_oversampling(&handle, BMP280_OVERSAMPLING_2X);
    bmp280_set_filter_coefficient      (&handle, BMP280_FILTER_4X);
    bmp280_set_standby_time            (&handle, BMP280_T_STANDBY_500US);
    bmp280_set_mode                    (&handle, BMP280_MODE_NORMAL);

    return 0;
}

int bmp280_driver_get_data(BMP280_Data_t *data) {
    float    temp     = 0.0f;
    uint32_t pressure = 0;
    float    altitude = 0.0f;

    if (bmp280_get_temperature(&handle, &temp)     != BMP280_ERROR_OK) return -1;
    if (bmp280_get_pressure   (&handle, &pressure) != BMP280_ERROR_OK) return -1;

    #if BMP280_INCLUDE_ALTITUDE
    bmp280_calculate_altitude_quick(&handle, &altitude, pressure);
    #endif

    data->temperature_C = temp;
    data->pressure_Pa   = (float)pressure;
    data->altitude_m    = altitude;
    return 0;
}