#ifndef BSP_SENSORS_H
#define BSP_SENSORS_H

#include <stdint.h>

typedef struct {
    float    temperature_C;
    float    pressure_Pa;
    float    altitude_m;
} BarometerData_t;

// The application only knows about this function
void barometer_sensor_get_data(BarometerData_t *data);
void setup_sensors();

#endif