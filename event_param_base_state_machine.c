#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <esp_log.h>

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