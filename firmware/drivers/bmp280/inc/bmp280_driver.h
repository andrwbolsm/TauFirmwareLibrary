#ifndef BMP280_DRIVER_H
#define BMP280_DRIVER_H

typedef struct {
    float    temperature_C;
    float    pressure_Pa;
    float    altitude_m;
} BMP280_Data_t;

int bmp280_driver_init(void);
int bmp280_driver_get_data(BMP280_Data_t *data);

#endif