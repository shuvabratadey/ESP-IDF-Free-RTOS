#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

typedef struct struc{
	int Member1;
	char Member2;
}xStruct;

void vPrintFunction(void *parameter);

void app_main()
{
	xStruct *obj = NULL;
	obj = (xStruct *)malloc(sizeof(xStruct));
	obj->Member1 = 5;
	obj->Member2 = 'S';
	xTaskCreate(vPrintFunction, "myTask", 2048, (void *) obj, 1, NULL);
}

void vPrintFunction(void *parameter)
{
	while(1)
	{
		printf("From myTask, 1st Parameter = %d\n", ((xStruct *)parameter)->Member1);
		printf("From myTask, 2nd Parameter = %c\n", ((xStruct *)parameter)->Member2);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
