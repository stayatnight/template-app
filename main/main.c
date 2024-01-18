/*
 * @Author: zwp
 * @Date: 2023-12-22 12:46:56
 * @LastEditors: zwp && 2234024575@qq.com
 * @LastEditTime: 2023-12-22 12:56:16
 * @FilePath: \template-app\main\main.c
 * @Description:
 *
 * Copyright (c) 2023 by ${2234023575@qq.com}, All Rights Reserved.
 */
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include "esp_log.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#define btn_pin GPIO_NUM_9
#define rgb_pin GPIO_NUM_8
static const char* TAG = "test";
// #define task_creat_null 1
// #define task_creat_hundle 2
// #define task_creat_pv  3
//#define task_gpio9_btn 4
//#define task_creat_from1 5
#define task_creat_pro 6
void task1(void* pv)
{
    while (1) {
        // 无限循环不带返回值的函数
        // 参数是void*
        ESP_LOGI(TAG, "hello task1");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void task2(void* pv)
{
    int i = 0;
    while (1) {
        i++;
        ESP_LOGI(TAG, "hello task2 %d", i);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        if (i == 1000)
            vTaskDelete(NULL);
        // DELTE NULL means delete self
    }
}
void task3(void* pv)
{
    for (;;) {
        ESP_LOGI(TAG, "hello task3");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void task4(void* pv)
{
    xTaskCreate(task3, "task1", 4096, NULL, 1, NULL);
    char* task_name;
    task_name = (char*)pv;
    for (;;) {
        // vPrintString(task_name);
        ESP_LOGI(TAG, "%s", task_name);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void button_task(void* pv)
{
    for (;;) {
        if (gpio_get_level(btn_pin) == 0) {
            gpio_set_level(rgb_pin, 1);
            ESP_LOGI(TAG, "btn 9 press");
        } else {
            gpio_set_level(rgb_pin, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}  // 按钮任务
void app_main(void)
{
// 不带句柄
#ifdef task_creat_null
    xTaskCreate(task1, "task1", 4096, NULL, 1, NULL);
    xTaskCreate(task2, "task2", 4096, NULL, 1, NULL);
#endif
#ifdef task_creat_hundle
    // 带句柄
    TaskHandle_t task1_handle;
    TaskHandle_t task2_handle;
    xTaskCreate(task1, "task1", 4096, NULL, 1, &task1_handle);
    xTaskCreate(task2, "task2", 4096, NULL, 1, &task2_handle);
#endif
// 使用任务参数
//  vTaskStartScheduler();
#ifdef task_creat_pv
    static const char* pv_task2 = "task4 is running";
    xTaskCreate(task4, "task4", 4096, (void*)pv_task2, 1, NULL);
#endif

#ifdef task_gpio9_btn
    //gpio_config_t button_conf = {
      //  .pin_bit_mask = (1ULL << btn_pin),  // 无符号长长整数
        //.mode         = GPIO_MODE_INPUT,
        //.intr_type    = GPIO_INTR_NEGEDGE,  // 
    //};
    //gpio_config(&button_conf);
   // gpio_glitch_filter_handle_t *btn_handle;
    //gpio_pin_glitch_filter_config_t btn_conf={
      // .gpio_num=btn_pin;
     //  .clk_src=

    //}
 //   gpio_new_pin_glitch_filter()
 //为什么设置了初始状态就不行了？
    gpio_config_t rgb_conf = {
        .pin_bit_mask = (1ULL << rgb_pin),
        .mode         = GPIO_MODE_OUTPUT,
        .intr_type    = GPIO_INTR_POSEDGE,
    };
    gpio_config(&rgb_conf);
    xTaskCreate(button_task, "button_task", 4096, NULL, 1, NULL);
#endif
#ifdef task_creat_from1//从一个任务函数，创建两个实例
static const char* pv_task3 = "task4 is running";
static const char* pv_task4 = "task5 is running";
    xTaskCreate(task4, "task4", 4096, (void*)pv_task3, 1, NULL);
    xTaskCreate(task4, "task5", 4096, (void*)pv_task4, 1, NULL);
#endif
#ifdef task_creat_pro//优先级数字越大，优先级越高
    xTaskCreate(task1, "task1", 4096, NULL, 1, NULL);
    xTaskCreate(task2, "task2", 4096, NULL, 2, NULL);
#endif
//可以通过函数在任务运行过程中修改任务优先级
}