#include "sensors.h"
#include <stdlib.h>

void BSP_Barometer_Read(BarometerData_t *data) {
    static float fake_alt = 0.0f;
    fake_alt += 1.5f; 
    
    data->altitude = fake_alt;
    data->temperature = 25.0f + ((float)rand()/(float)RAND_MAX);
}