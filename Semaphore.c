#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xSemaphore = NULL;

TaskHandle_t myTaskHandle = NULL;
TaskHandle_t myTaskHandle2 = NULL;

void Demo_Task(void *arg)
{
    while(1){
        printf("Message Sent! [%ld] \n", xTaskGetTickCount());
        xSemaphoreGive(xSemaphore);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Demo_Task2(void *arg)
{
    while(1){
     if(xSemaphoreTake(xSemaphore, portMAX_DELAY))
     {
      printf("Received Message [%ld] \n", xTaskGetTickCount());
     }
    }
}

void app_main(void)
{
   xSemaphore = xSemaphoreCreateBinary();
   xTaskCreate(Demo_Task, "Demo_Task", 4096, NULL, 10, &myTaskHandle);
   xTaskCreatePinnedToCore(Demo_Task2, "Demo_Task2", 4096, NULL,10, &myTaskHandle2, 1);
 }