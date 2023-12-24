#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

int num = 5;

void vPrintFunction(void *parameter);

void app_main()
{
	xTaskCreate(vPrintFunction, "myTask", 2048, (void *) &num, 1, NULL);
}

void vPrintFunction(void *parameter)
{
	while(1)
	{
		printf("From myTask, Parameter = %d\n", *((int *)parameter));
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
