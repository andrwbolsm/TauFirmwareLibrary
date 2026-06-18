#include "sensors.h"
#include "bmp280_driver.h"   

static void setup_barometer(void);

void setup_sensors(void) {
    setup_barometer();
}

static void setup_barometer(void) {
    if (bmp280_driver_init() != 0) {
        // handle error
    }
}

void barometer_sensor_get_data(BarometerData_t *data) {
    BMP280_Data_t bmp_data;
    
    bmp280_driver_get_data(&bmp_data);

    data->pressure_Pa  = bmp_data.pressure_Pa;
    data->temperature_C  = bmp_data.temperature_C;
    data->altitude_m     = bmp_data.altitude_m;
}