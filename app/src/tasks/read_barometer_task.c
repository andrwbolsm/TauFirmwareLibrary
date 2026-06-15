#include "read_barometer_task.h"
#include "sensors.h" 
#include <stdio.h>

void readBarometer (void *pvParameters)
{
    BarometerData_t barometerData;
    
	while(1)
	{
        BSP_Barometer_Read(&barometerData);
        printf("Altitude atual: %.2f metros\n", barometerData.altitude);

		vTaskDelay(pdMS_TO_TICKS(300));
	}
}
