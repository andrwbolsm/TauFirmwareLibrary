#include "FreeRTOS.h"
#include "task.h"

#include "sensors.h"
#include "read_barometer_task.h" 

void createTasks(void);

int main(void){
    setup_sensors();

    createTasks();

    vTaskStartScheduler();

    while(1)
	{
		// The program should never be here...
	}

    return 0;
}

void createTasks(void)
{
    xTaskCreate(readBarometer, "Task_1", 256, NULL, 1, NULL);
}