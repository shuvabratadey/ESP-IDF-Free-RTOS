/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

#include "driver/gpio.h"
#include "esp_log.h"
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

void my_task();



void app_main(void)
{
	TaskHandle_t myTaskHandeler = NULL;
	xTaskCreate(my_task, "myTask", 1024, NULL, 1, &myTaskHandeler);
	vTaskDelay(3000 / portTICK_PERIOD_MS);

	if(myTaskHandeler != NULL)
	{
		vTaskDelete(myTaskHandeler);
	}

	printf("Task has been deleted.");
	while(1)
	{
		printf("\nFrom main function.\n");
		fflush(stdout);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}

void  my_task(void *pvParam)
{
	while(1)
	{
		printf("Hello Shuva, What's up?\n");
		fflush(stdout);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
