#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

int num[] = {5, 6, 7, 8};

void vPrintFunction(void *parameter);

void app_main()
{
	xTaskCreate(vPrintFunction, "myTask", 2048, (void *) num, 1, NULL);
}

void vPrintFunction(void *parameter)
{
	while(1)
	{
		printf("From myTask, 1st Parameter = %d\n", *((int *)parameter));
		printf("From myTask, 2nd Parameter = %d\n", *((int *)parameter + 1));
		printf("From myTask, 3rd Parameter = %d\n", *((int *)parameter + 2));
		printf("From myTask, 4th Parameter = %d\n", *((int *)parameter + 3));
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
