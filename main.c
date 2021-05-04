/* OTA example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
//#include "protocol_examples_common.h"
#include "string.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_flash_encrypt.h"
//#include "protocol_examples_common.h"

#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif

static const char *TAG = "simple_ota_example";


esp_err_t nvs_secure_initialize() {
    static const char *nvs_tag = "nvs";
    esp_err_t err = ESP_OK;

    // 1. find partition with nvs_keys
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS,
                                                                "nvs_key");
    if (partition == NULL) {
        ESP_LOGE(nvs_tag, "Could not locate nvs_key partition. Aborting.");
        return ESP_FAIL;
    }

    // 2. read nvs_keys from key partition
    nvs_sec_cfg_t cfg;
    if (ESP_OK != (err = nvs_flash_read_security_cfg(partition, &cfg))) {
        ESP_LOGE(nvs_tag, "Failed to read nvs keys (rc=0x%x)", err);
        return err;
    }

    // 3. initialize nvs partition
    if (ESP_OK != (err = nvs_flash_secure_init(&cfg))) {
        ESP_LOGE(nvs_tag, "failed to initialize nvs partition (err=0x%x). Aborting.", err);
        return err;
    };

    return err;
}


void nvs_read_file() {
    esp_err_t err = ESP_OK;
    size_t required_size;
    nvs_handle my_handle;
    char* tmp;
	err = nvs_open("nvs", NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		printf("Error opening NVS handle!\n");

    } else {
        size_t count;
        err = nvs_get_used_entry_count( my_handle, &count );
	    if (err != ESP_OK) {
		    printf("nvs_get_used_entry_count error: %d\n", err);
        }

        printf( "nvs_get_used_entry_count: %d\n", count );

        err = nvs_get_str(my_handle, "TEST_FILE", NULL, &required_size);
        if(err == ESP_OK) {
            tmp = malloc(required_size + 1);
            nvs_get_str(my_handle, "TEST_FILE", tmp, &required_size);
            tmp[required_size] = "\0";
            printf( "********************\n" );
            printf( "%s\n", tmp );
            printf( "********************\n" );
            free( tmp );

        } else {
            printf("nvs - TEST_FILE not found on memory\n");
        }
    }
}


void app_main(void)
{
    // Initialize NVS.
    esp_err_t err = nvs_secure_initialize();
    if (err != ESP_OK) {
        ESP_LOGE("main", "Failed to initialize nvs (rc=0x%x). Halting.", err);
        while(1) { vTaskDelay(100); }
    }

    nvs_read_file();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}


/***************************************/
/***************************************/
extern void esp_vApplicationTickHook();
void IRAM_ATTR vApplicationTickHook()
{
    esp_vApplicationTickHook();
}


extern void esp_vApplicationIdleHook();
void vApplicationIdleHook()
{
    esp_vApplicationIdleHook();
}


void 
vApplicationDaemonTaskStartupHook( void )
{

}

/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
 * used by the Idle task. */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    /* If the buffers to be provided to the Idle task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/**
 * @brief This is to provide the memory that is used by the RTOS daemon/time task.
 *
 * If configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetTimerTaskMemory() to provide the memory that is
 * used by the RTOS daemon/time task.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    /* If the buffers to be provided to the Timer task are declared inside this
     * function then they must be declared static - otherwise they will be allocated on
     * the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle
     * task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
     * Note that, as the array is necessarily of type StackType_t,
     * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

