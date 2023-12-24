#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xSemaphore = NULL;

TaskHandle_t myTaskHandle = NULL;
TaskHandle_t myTaskHandle2 = NULL;

void Demo_Task(void *arg)
{
    while (1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        printf("Message Sent! [%ld Sec] \n", pdTICKS_TO_MS(xTaskGetTickCount())/1000);
        vTaskDelay(pdMS_TO_TICKS(3000));
        xSemaphoreGive(xSemaphore);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void Demo_Task2(void *arg)
{
    while (1)
    {
        xSemaphoreTake(xSemaphore, portMAX_DELAY);
        printf("\033[0;34mReceived Message [%ld Sec] \n\033[0;37m", pdTICKS_TO_MS(xTaskGetTickCount())/1000);
        vTaskDelay(pdMS_TO_TICKS(2000));
        xSemaphoreGive(xSemaphore);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemaphore);
    xTaskCreate(Demo_Task, "Demo_Task", 4096, NULL, 10, &myTaskHandle);
    vTaskDelay(pdMS_TO_TICKS(500));
    xTaskCreatePinnedToCore(Demo_Task2, "Demo_Task2", 4096, NULL, 10, &myTaskHandle2, 1);
}
