#include <stdio.h>
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