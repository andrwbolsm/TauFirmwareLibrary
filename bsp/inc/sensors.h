#ifndef BSP_SENSORS_H
#define BSP_SENSORS_H

#include <stdint.h>

typedef struct {
    float altitude;
    float temperature;
} BarometerData_t;

// The application only knows about this function
void BSP_Barometer_Read(BarometerData_t *data);

#endif