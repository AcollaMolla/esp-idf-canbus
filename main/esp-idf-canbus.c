#include <stdio.h>
#include "driver/twai.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG_TWAI = "CAN";

void task_TWAI_receive(void *pvParameters);

void setup_twai_driver(void){
    twai_general_config_t g_config = {
        .mode = TWAI_MODE_NORMAL,
        .tx_io = (gpio_num_t)4,
        .rx_io = (gpio_num_t)5,
        .clkout_io = (gpio_num_t)TWAI_IO_UNUSED,
        .bus_off_io = (gpio_num_t)TWAI_IO_UNUSED,
        .tx_queue_len = 10,
        .rx_queue_len = 65,
        .alerts_enabled = TWAI_ALERT_ALL,
        .clkout_divider = 0
    };    
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_125KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if(twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK){
        ESP_LOGI(TAG_TWAI, "Driver installed!\n");
    }else{
        ESP_LOGI(TAG_TWAI, "Failed to install driver\n");
    }

    //Start TWAI driver
    if(twai_start() == ESP_OK){
        ESP_LOGI(TAG_TWAI, "Driver started!\n");
    }else{
        ESP_LOGI(TAG_TWAI, "Failed to start driver\n");
    }
}

void app_main(void)
{
    //Let user know we are in app_main()
    char *thisTaskName = pcTaskGetName(NULL);
    ESP_LOGI(thisTaskName, "Hello, starting up!\n");

    //Set up the TWAI driver
    setup_twai_driver();

    //Create tasks
    //This task will likely cause an error later on because 1024 bytes is not enough memory for
    //65 CAN-bus frames which are roughly 16 bytes in size each. If the queue is entirely filled up
    //then this task will consume around 1040 bytes, thus causing a kernel panic
    xTaskCreate(task_TWAI_receive, "CAN Receive", 1024, NULL, 1, NULL);
}

void task_TWAI_receive(void *pvParameters){

    while(1){

        //Any CAN-bus frames read from the queue will be put here
        twai_message_t rx_frame;

        //Check if the queue contains any new CAN-bus frames
        if(twai_receive(&rx_frame, pdMS_TO_TICKS(1000)) == ESP_OK){
            
            //Print ID and DLC of frame
            printf("%x, %x: ", rx_frame.identifier, rx_frame.data_length_code);
            for(int i=0;i<rx_frame.data_length_code;i++){
                printf("%x ", rx_frame.data[i]);
            }
            printf("\n");
        }
    }
}
