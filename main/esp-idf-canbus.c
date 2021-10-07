#include <stdio.h>
#include "driver/twai.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char* TAG_TWAI = "CAN";

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

}
