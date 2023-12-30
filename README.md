# ESP-IDF-Free-RTOS

## What is Free RTOS ?

FreeRTOS is an open-source real-time operating system (RTOS) designed for embedded systems. It is known for its small footprint, portability, and scalability, making it a popular choice for a wide range of microcontroller-based applications. Here are some key aspects of FreeRTOS:

* Architecture and Design
* Kernel Features
* Task Management
* Synchronization and Communication
* Memory Management
* Portability
* Community Support
* Licensing
* Real-Time Performance
* Ecosystem
  
Overall, FreeRTOS is a powerful and widely used open-source RTOS that continues to evolve, meeting the needs of developers working on diverse embedded applications.

## This are some basic Example of Free RTOS Codes in ESP-IDF

## Include this Libraries First

``` c
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
```

### Task Create

``` c
void my_task(void *pvParam)
{
 while(1)
 {
  printf("Hello Shuva, What's up?\n");
  fflush(stdout);
  vTaskDelay(500 / portTICK_PERIOD_MS);
 }
}

void app_main(void)
{
 xTaskCreate(my_task, "myTask", 1024, NULL, 1, NULL);
}
```

### Task Delete

```c
void my_task(void *pvParam)
{
 while(1)
 {
  printf("Hello Shuva, What's up?\n");
  fflush(stdout);
  vTaskDelay(500 / portTICK_PERIOD_MS);
 }
}

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

```

### Task Input Parameter

```c
void vPrintFunction(void *parameter)
{
 while(1)
 {
  printf("From myTask, Parameter = %d\n", *((int *)parameter));
  vTaskDelay(1000/portTICK_PERIOD_MS);
 }
}

void app_main()
{
    int num = 5;
 xTaskCreate(vPrintFunction, "myTask", 2048, (void *) &num, 1, NULL);
}
```

### Task Input Parameter as array

```c
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

void app_main()
{
    int num[] = {5, 6, 7, 8};
 xTaskCreate(vPrintFunction, "myTask", 2048, (void *) num, 1, NULL);
}
```

### Task Input Parameter as struct

```c
typedef struct struc{
int Member1;
 char Member2;
}xStruct;

void vPrintFunction(void *parameter)
{
 while(1)
 {
  printf("From myTask, 1st Parameter = %d\n", ((xStruct *)parameter)->Member1);
  printf("From myTask, 2nd Parameter = %c\n", ((xStruct *)parameter)->Member2);
  vTaskDelay(1000/portTICK_PERIOD_MS);
 }
}

void app_main()
{
 xStruct *obj = NULL;
 obj = (xStruct *)malloc(sizeof(xStruct));
 obj->Member1 = 5;
 obj->Member2 = 'S';
 xTaskCreate(vPrintFunction, "myTask", 2048, (void *) obj, 1, NULL);
}
```

# State Machine

In FreeRTOS, as with any real-time operating system (RTOS), a state machine is a design pattern often used to model the behavior of a system. A state machine consists of a set of states, events, and transitions. In the context of FreeRTOS, these elements are typically implemented using tasks, queues, and other synchronization mechanisms provided by the RTOS.

Here's a general approach to implementing a state machine in FreeRTOS:

### Event Base State Machine

```c
typedef enum{
    APP_SM_EVENT_ENTRY,
    APP_EVENT_SYSTEM_INIT,
    APP_EVENT_SYSTEM_NEXT,
    APP_SM_EVENT_EXIT,
} app_sm_t;

typedef enum {
    APP_SM_STATUS_HANDLED,
    APP_SM_STATUS_IGNORED,
    APP_SM_STATUS_TRAN,
} app_smStatus_t;

QueueHandle_t gEventQH;
#define APP_CONFIG_EVENT_QUEUE_SIZE 10

typedef struct app_smInst app_smInst_t;

typedef app_smStatus_t (*app_smStateHandle_t)(app_smInst_t *const pInstance, app_sm_t pParam);
struct app_smInst {
    app_smStateHandle_t active;
    app_smStateHandle_t nextState;
};
static app_smInst_t gAppSmInt;

static void AppPostEvent(app_sm_t event);
static void AppDispatcher();

// Create State Machine's Functuions
app_smStatus_t AppState1(app_smInst_t *const pInstance, app_sm_t pEventParam);
app_smStatus_t AppState2(app_smInst_t *const pInstance, app_sm_t pEventParam);

app_smStatus_t AppState1(app_smInst_t *const pInstance, app_sm_t pEventParam) {
    app_smStatus_t returnStatus = APP_SM_STATUS_HANDLED;
    switch (pEventParam) {
        case APP_SM_EVENT_ENTRY: {
            ESP_LOGI("TAG", "APP_SM_EVENT_ENTRY 1");
            AppPostEvent(APP_EVENT_SYSTEM_INIT);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_INIT: {
            ESP_LOGI("TAG", "APP_EVENT_SYSTEM_INIT 1");
            AppPostEvent(APP_EVENT_SYSTEM_NEXT);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_NEXT: {
            ESP_LOGI("TAG", "APP_EVENT_SYSTEM_NEXT 1");
            pInstance->nextState = AppState2;
            returnStatus = APP_SM_STATUS_TRAN;
            break;
        }
  
        case APP_SM_EVENT_EXIT: {
            ESP_LOGI("TAG", "APP_SM_EVENT_EXIT 1");
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        default: {
            ESP_LOGE("TAG", "%s : %d : Ignored event 1 %d : %d", __func__, __LINE__, pEventParam, returnStatus);
            returnStatus = APP_SM_STATUS_IGNORED;
            break;
        }
    }
    return returnStatus;
}

app_smStatus_t AppState2(app_smInst_t *const pInstance, app_sm_t pEventParam) {
    app_smStatus_t returnStatus = APP_SM_STATUS_HANDLED;
    switch (pEventParam) {
        case APP_SM_EVENT_ENTRY: {
            ESP_LOGW("TAG", "APP_SM_EVENT_ENTRY 2");
            AppPostEvent(APP_EVENT_SYSTEM_INIT);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_INIT: {
            ESP_LOGW("TAG", "APP_EVENT_SYSTEM_INIT 2");
            AppPostEvent(APP_EVENT_SYSTEM_NEXT);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_NEXT: {
            ESP_LOGW("TAG", "APP_EVENT_SYSTEM_NEXT 2");
            pInstance->nextState = AppState1;
            returnStatus = APP_SM_STATUS_TRAN;
            break;
        }
  
        case APP_SM_EVENT_EXIT: {
            ESP_LOGW("TAG", "APP_SM_EVENT_EXIT 2");
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        default: {
            ESP_LOGE("TAG", "%s : %d : Ignored event 2 %d : %d", __func__, __LINE__, pEventParam, returnStatus);
            returnStatus = APP_SM_STATUS_IGNORED;
            break;
        }
    }
    return returnStatus;
}

static void AppPostEvent(app_sm_t event) {
    if (xQueueSend(gEventQH, &event, 100) != pdTRUE) {
        ESP_LOGW("TAG", " %s : %d : Failed to post event", __func__, __LINE__);
    }
}

static void AppDispatcher(void *pvParam) {
    app_sm_t event;
    app_smStatus_t ret;
    
    while (1) {
        xQueueReceive(gEventQH, &event, portMAX_DELAY);
        ret = gAppSmInt.active(&gAppSmInt, event);
        if (ret == APP_SM_STATUS_TRAN) {
            event = APP_SM_EVENT_EXIT;
            gAppSmInt.active(&gAppSmInt, event);
            if (gAppSmInt.nextState != NULL) {
                gAppSmInt.active = gAppSmInt.nextState;
                event = APP_SM_EVENT_ENTRY;
                gAppSmInt.active(&gAppSmInt, event);
            } else {
                ESP_LOGE("TAG", "%s : %d : State Change Requested, but NextSate is NULL", __func__, __LINE__);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    gEventQH = xQueueCreate(APP_CONFIG_EVENT_QUEUE_SIZE, sizeof(app_sm_t));
    gAppSmInt.active = AppState1;
    app_sm_t event = APP_SM_EVENT_ENTRY;
    gAppSmInt.active(&gAppSmInt, event);
    gAppSmInt.nextState = NULL;
    xTaskCreate(AppDispatcher, "AppDispatcherTask", 2048, NULL, 1, NULL);
}
```

### Event Parameter Base State Machine

```c
typedef enum{
 APP_SM_EVENT_ENTRY,
    APP_EVENT_SYSTEM_INIT,
    APP_EVENT_SYSTEM_NEXT,
 APP_SM_EVENT_EXIT,
} app_sm_t;

typedef struct {
    app_sm_t event;
    union {
        struct {
            uint8_t param1;
            uint16_t param2;
        } var1;
        struct {
            char *param3;
            uint16_t param4;
        } var2;
    };
} app_eventParam_t;

typedef enum {
    APP_SM_STATUS_HANDLED,
    APP_SM_STATUS_IGNORED,
    APP_SM_STATUS_TRAN,
} app_smStatus_t;

QueueHandle_t gEventQH;
#define APP_CONFIG_EVENT_QUEUE_SIZE 10

typedef struct app_smInst app_smInst_t;

typedef app_smStatus_t (*app_smStateHandle_t)(app_smInst_t *const pInstance, app_eventParam_t *pEventParam);
struct app_smInst {
    app_smStateHandle_t active;
    app_smStateHandle_t nextState;
};
static app_smInst_t gAppSmInt;

static void AppPostEvent(app_eventParam_t *pEventParam);
static void AppDispatcher();

// Create State Machine's Functuions
app_smStatus_t AppState1(app_smInst_t *const pInstance, app_eventParam_t *pEventParam);
app_smStatus_t AppState2(app_smInst_t *const pInstance, app_eventParam_t *pEventParam);

app_smStatus_t AppState1(app_smInst_t *const pInstance, app_eventParam_t *pEventParam) {
    app_eventParam_t eventParam;
    app_smStatus_t returnStatus = APP_SM_STATUS_HANDLED;
    switch (pEventParam->event) {
        case APP_SM_EVENT_ENTRY: {
            ESP_LOGI("TAG", "APP_SM_EVENT_ENTRY 1 (param 1,2 posted)");
            eventParam.event = APP_EVENT_SYSTEM_INIT;
            eventParam.var1.param1 = 0x10;
            eventParam.var1.param2 = 0x6F85;
            AppPostEvent(&eventParam);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_INIT: {
            ESP_LOGI("TAG", "APP_EVENT_SYSTEM_INIT 1 (param1: 0x%X, param2: 0x%X)", pEventParam->var1.param1, pEventParam->var1.param2);
            eventParam.event = APP_EVENT_SYSTEM_NEXT;
            AppPostEvent(&eventParam);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_NEXT: {
            ESP_LOGI("TAG", "APP_EVENT_SYSTEM_NEXT 1");
            pInstance->nextState = AppState2;
            returnStatus = APP_SM_STATUS_TRAN;
            break;
        }
  
        case APP_SM_EVENT_EXIT: {
            ESP_LOGI("TAG", "APP_SM_EVENT_EXIT 1");
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        default: {
            ESP_LOGE("TAG", "%s : %d : Ignored event 1 %d : %d", __func__, __LINE__, pEventParam->event, returnStatus);
            returnStatus = APP_SM_STATUS_IGNORED;
            break;
        }
    }
    return returnStatus;
}

app_smStatus_t AppState2(app_smInst_t *const pInstance, app_eventParam_t *pEventParam) {
    app_eventParam_t eventParam;
    app_smStatus_t returnStatus = APP_SM_STATUS_HANDLED;
    switch (pEventParam->event) {
        case APP_SM_EVENT_ENTRY: {
            ESP_LOGW("TAG", "APP_SM_EVENT_ENTRY 2 (param 3,4 posted)");
            eventParam.event = APP_EVENT_SYSTEM_INIT;
            eventParam.var2.param3 = (char *)malloc(10 * sizeof(char));
            memset(eventParam.var2.param3, 0, 10 * sizeof(char));
            strcpy(eventParam.var2.param3, "Shuva is good");
            eventParam.var2.param4 = 0x2048;
            AppPostEvent(&eventParam);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_INIT: {
            ESP_LOGW("TAG", "APP_EVENT_SYSTEM_INIT 2 (param3: %s, param4: 0x%X)", pEventParam->var2.param3, pEventParam->var2.param4);
            free(pEventParam->var2.param3);
            pEventParam->var2.param3 = NULL;
            eventParam.event = APP_EVENT_SYSTEM_NEXT;
            AppPostEvent(&eventParam);
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        case APP_EVENT_SYSTEM_NEXT: {
            ESP_LOGW("TAG", "APP_EVENT_SYSTEM_NEXT 2");
            pInstance->nextState = AppState1;
            returnStatus = APP_SM_STATUS_TRAN;
            break;
        }
  
        case APP_SM_EVENT_EXIT: {
            ESP_LOGW("TAG", "APP_SM_EVENT_EXIT 2");
            returnStatus = APP_SM_STATUS_HANDLED;
            break;
        }

        default: {
            ESP_LOGE("TAG", "%s : %d : Ignored event 2 %d : %d", __func__, __LINE__, pEventParam->event, returnStatus);
            returnStatus = APP_SM_STATUS_IGNORED;
            break;
        }
    }
    return returnStatus;
}

static void AppPostEvent(app_eventParam_t *pEventParam) {
    if (xQueueSend(gEventQH, pEventParam, 100) != pdTRUE) {
        ESP_LOGW("TAG", " %s : %d : Failed to post event", __func__, __LINE__);
    }
}

static void AppDispatcher(void *pvParam) {
    app_eventParam_t eventParam;
    app_smStatus_t ret;
    
    while (1) {
        xQueueReceive(gEventQH, &eventParam, portMAX_DELAY);
        ret = gAppSmInt.active(&gAppSmInt, &eventParam);
        if (ret == APP_SM_STATUS_TRAN) {
            eventParam.event = APP_SM_EVENT_EXIT;
            gAppSmInt.active(&gAppSmInt, &eventParam);
            if (gAppSmInt.nextState != NULL) {
                gAppSmInt.active = gAppSmInt.nextState;
                eventParam.event = APP_SM_EVENT_ENTRY;
                gAppSmInt.active(&gAppSmInt, &eventParam);
            } else {
                ESP_LOGE("TAG", "%s : %d : State Change Requested, but NextSate is NULL", __func__, __LINE__);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    app_eventParam_t eventParam;
    gEventQH = xQueueCreate(APP_CONFIG_EVENT_QUEUE_SIZE, sizeof(app_eventParam_t));
    gAppSmInt.active = AppState1;
    eventParam.event = APP_SM_EVENT_ENTRY;
    gAppSmInt.active(&gAppSmInt, &eventParam);
    gAppSmInt.nextState = NULL;
    xTaskCreate(AppDispatcher, "AppDispatcherTask", 2048, NULL, 1, NULL);
}
```

## Semaphore Code

```c
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
```

This are some basic example of state machines and the actual implementation may vary depending on the complexity of your system and specific requirements. The key idea is to leverage FreeRTOS tasks and synchronization mechanisms to create a well-organized and efficient state machine for your embedded application.
