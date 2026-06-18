#include "read_barometer_task.h"
#include "sensors.h" 
#include <stdio.h>

void readBarometer (void *pvParameters)
{
    BarometerData_t barometerData;
    
	while(1)
	{
        barometer_sensor_get_data(&barometerData);

		#ifdef DEBUG_PC
        printf("ALTITUDE: %.2f m; TEMPERATURE: %.2f; PRESSURE : %.2f Pa\n", barometerData.altitude_m, barometerData.temperature_C, barometerData.pressure_Pa);
		#endif

		vTaskDelay(pdMS_TO_TICKS(38));
	}
}
