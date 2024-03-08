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
#include <freertos/FreeRTOSConfig.h>
// #include <freertos/task.h>
#include "freertos/semphr.h"
#include <freertos/timers.h>
#include "esp_log.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_app_trace.h"
#include "string.h"
#include <freertos/portmacro.h>
#include "test_queue.h"
#include <freertos/projdefs.h>
#include"config.h"
// #include <portmacro.h>
#define btn_pin GPIO_NUM_9
#define rgb_pin GPIO_NUM_8
volatile uint32_t  ulIdleCycleCount = 0UL;
static const char* TAG              = "test";
// #define task_creat_null 1
// #define task_creat_hundle 2
// #define task_creat_pv  3
// #define task_gpio9_btn 4
// #define task_creat_from1 5
// #define task_creat_pro 6
// #define task_other_state 7
// #define task_idle_fuc 8
// #define task_delete_ 9
// #define resource_critical 10
// #define resource_mutex 11
// #define resource_gate_keeper 12
// #define resource 10
// #define resource 10
//#define queue_test 15
//#define softTimer  16
//#define timer_test 1 

char              down_buf[32];
uint32_t*         number;
size_t            sz = 32;
SemaphoreHandle_t xMutex;
void              task1(void* pv)
{
    while (1) {
        // 无限循环不带返回值的函数
        // 参数是void*
        ESP_LOGI(TAG, "hello task1");
        //   vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
void task2(void* pv)
{
    int i = 0;
    while (1) {
        i++;
        ESP_LOGI(TAG, "hello task2 %d", i);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        //        if (i == 1000)
        //          vTaskDelete(NULL);
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
    // xTaskCreate(task3, "task1", 4096, NULL, 1, NULL);
    char* task_name;
    task_name = (char*)pv;
    for (;;) {
        // vPrintString(task_name);
        ESP_LOGI(TAG, "%s", task_name);
        vTaskDelay(10 / portTICK_PERIOD_MS);  // 此函数将使得任务处于阻塞状态
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
void task_suspend(void* pv)
{
    int i = 0;
    int b = 0;
    for (;;) {
        i++;
        b++;
        for (b = 0; b <= 100; b++) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "hello task3");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (i == 1000)
            vTaskSuspend(NULL);  // 计数1000然后挂起
    }
}

void task_suspend_utill(void* pv)
{
    int              i = 0;
    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    int              b          = 0;
    for (;;) {
        i++;
        b++;
        for (b = 0; b <= 100; b++) {
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }

        ESP_LOGI(TAG, "hello task3");
        vTaskDelayUntil(
            &xLastWakeTime,
            xFrequency);  // 相对延迟从vtask执行时刻开始计算,没有计算前面的一段时间，而vTaskDelayUtill是可以计算的
        if (i == 1000)
            vTaskSuspend(NULL);  // 计数1000然后挂起
    }
}

void vApplicationIdleHook(void)  // 类似于qt里面的槽函数，并且它是自动创建的
{
    ulIdleCycleCount++;
}
void task_idlehook_printf(void* pv)
{
    int              i = 0;
    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    int              b          = 0;
    for (;;) {
        i++;
        b++;
        // 执行101次循环
        for (b = 0; b <= 100; b++) {
            // 每次循环延迟10毫秒
            vTaskDelay(10 / portTICK_PERIOD_MS);
            // 打印空闲钩子函数的执行次数
            ESP_LOGI(TAG, "idle hook is %lu", ulIdleCycleCount);
        }
        // 打印hello task3
        ESP_LOGI(TAG, "hello task3");
        // 相对延迟从vTask执行时刻开始计算，没有计算前面的一段时间，而vTaskDelayUntil是可以计算的
        vTaskDelayUntil(
            &xLastWakeTime,
            xFrequency);  // 相对延迟从vtask执行时刻开始计算,没有计算前面的一段时间，而vTaskDelayUtill是可以计算的
        // 计数达到1000后挂起任务
        if (i == 1000)
            vTaskSuspend(NULL);  // 计数1000然后挂起
    }
}

/**
 * @brief 删除任务
 *
 * 根据传入的参数，删除一个任务。
 *
 * @param pv 指向任务名称的指针
 */
void task_delete(void* pv)
{
    // xTaskCreate(task3, "task1", 4096, NULL, 1, NULL);
    char* task_name;
    //  int i=0;
    task_name = (char*)pv;
    for (;;) {
        // vPrintString(task_name);
        vTaskSuspend(NULL);
        ESP_LOGI(TAG, "%s", task_name);
        vTaskDelay(10 / portTICK_PERIOD_MS);  // 此函数将使得任务处于阻塞状态
    }
}
void task_cirtical_suspend(void* pv)
{
    char* task_name;
    task_name = (char*)pv;
    for (;;) {
        //   taskENTER_CRITICAL();
        vTaskEndScheduler();  // 挂起进程调度器的方式来解决问题
        // vPrintString(task_name);
        ESP_LOGI(TAG, "%s", task_name);
        vTaskDelay(10 / portTICK_PERIOD_MS);  // 此函数将使得任务处于阻塞状态
                                              //        taskEXIT_CRITICAL();
        vTaskStartScheduler();
    }
}

/**
 * @brief 任务1的互斥锁函数
 *
 * 使用互斥锁来保护共享资源，确保任务独占访问。
 *
 * @param pv 指向任务名称的指针
 */
void task1_mutex(void* pv)
{
    char* task_name;
    task_name = (char*)pv;
    for (;;) {
        //   taskENTER_CRITICAL();
        xSemaphoreTake(xMutex, portMAX_DELAY);
        {
            // vPrintString(task_name);
            ESP_LOGI(
                TAG, "%s",
                task_name);  // 本质上就是使用之前加上锁，保证独占，使用之后让出资源，实际上和之前的写法一样，也是一个临界区
            vTaskDelay(10 / portTICK_PERIOD_MS);  // 此函数将使得任务处于阻塞状态
            //        taskEXIT_CRITICAL();
        }
        xSemaphoreGive(xMutex);
    }
}

/**
 * @brief 任务2的互斥锁函数
 *
 * 该函数作为任务2的入口函数，使用互斥锁保护共享资源。
 *
 * @param pv 指向任务名称的指针
 */
void task2_mutex(void* pv)
{
    char* task_name;
    task_name = (char*)pv;
    for (;;) {
        //   taskENTER_CRITICAL();
        xSemaphoreTake(xMutex, portMAX_DELAY);
        {
            // vPrintString(task_name);
            ESP_LOGI(TAG, "%s", task_name);
            vTaskDelay(10 / portTICK_PERIOD_MS);  // 此函数将使得任务处于阻塞状态
            //        taskEXIT_CRITICAL();
        }
        xSemaphoreGive(xMutex);
    }
}
/**
 * @brief 任务守护函数
 *
 * 这是一个任务守护函数，用于管理任务的执行。
 */
void task_gate_keeper()
{
    int16_t x;
}
/**
 * @brief 打印字符串
 *
 * 在互斥锁保护下打印给定的字符串。
 *
 * @param pcString 要打印的字符串
 */
static void prvNewPrintString(const char* pcString)
{
    /*互斥锁是在调度器启动之前创建的，所以在任务执行时已经存在尝试获取互斥锁，如果互斥锁不能立即可用，则无限期阻塞以等待它。对xSemaphoreTake()的调用只有在成功获得互斥锁时才会返回。所以不需要检查函数的返回值。如果使用了任何其他延迟周期，那么代码必须在访问共享资源(在本例中是标准输出)之前检查xSemaphorerake()是否返回pdTRUE。正如本书前面提到的，不建议在生产代码中使用无限期超时。*/
    xSemaphoreTake(xMutex, portMAX_DELAY);
    {
        /*只有成功获得互斥锁后，才会执行下面的行。现在可以自由访问标准输出，因为在任何时候只有一个任务可以拥有互斥锁。*/
        printf("%s", pcString);
        fflush(stdout);

        /* 互斥锁必须返回! */
    }
    xSemaphoreGive(xMutex);
}
/**
 * @brief 打印任务函数
 *
 * 该函数是一个打印任务，用于无限循环地打印传入的字符串，并在每次打印后等待一段时间。
 *
 * @param pvParameters 任务参数指针
 */
static void prvPrintTask(void* pvParameters)
{
    char*            pcStringToPrint;
    const TickType_t xMaxBlockTimeTicks = 0x20;
    /*创建该任务的两个实例。任务打印的字符串“使用任务的参数传递给任务”。参数被转换为所需的类型。*/
    pcStringToPrint = (char*)pvParameters;

    for (;;) {
        /*使用新定义的函数输出字符串。*/
        prvNewPrintString(pcStringToPrint);
        /*等待一个伪随机时间。请注意rand()不一定是可重入的，但在这种情况下，它实际上并不重要，因为代码并不关心返回的值是什么。在更安全的应用程序中，应该使用已知可重入的rand()版本——或者应该在临界区段保护rand()的调用。*/
        vTaskDelay((rand() % xMaxBlockTimeTicks));
    }
}
static void task_queue_recive(void* pv) {}
/**
 * @brief 发送任务
 *
 * 这是一个静态函数，用于执行发送任务。
 */
static void vSendTask()
{
    int32_t    valueToSend;
    BaseType_t xStatus;
}
static void task_queue_send(void* pv) {}
void        app_main(void)
{
// 不带句柄
#ifdef task_creat_null
    xTaskCreate(task1, "task1", 4096, NULL, 1, NULL);
    xTaskCreate(task2, "task2", 4096, NULL, 1, NULL);
#endif
#ifdef task_creat_hundle
    // 带句柄
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
    // gpio_config_t button_conf = {
    //   .pin_bit_mask = (1ULL << btn_pin),  // 无符号长长整数
    //.mode         = GPIO_MODE_INPUT,
    //.intr_type    = GPIO_INTR_NEGEDGE,  //
    //};
    // gpio_config(&button_conf);
    // gpio_glitch_filter_handle_t *btn_handle;
    // gpio_pin_glitch_filter_config_t btn_conf={
    // .gpio_num=btn_pin;
    //  .clk_src=

    //}
    //   gpio_new_pin_glitch_filter()
    // 为什么设置了初始状态就不行了？
    gpio_config_t rgb_conf = {
        .pin_bit_mask = (1ULL << rgb_pin),
        .mode         = GPIO_MODE_OUTPUT,
        .intr_type    = GPIO_INTR_POSEDGE,
    };
    gpio_config(&rgb_conf);
    xTaskCreate(button_task, "button_task", 4096, NULL, 1, NULL);
#endif
#ifdef task_creat_from1  // 从一个任务函数，创建两个实例
    static const char* pv_task3 = "task4 is running";
    static const char* pv_task4 = "task5 is running";
    xTaskCreate(task4, "task4", 4096, (void*)pv_task3, 1, NULL);
    xTaskCreate(task4, "task5", 4096, (void*)pv_task4, 1, NULL);
#endif
#ifdef task_creat_pro  // 优先级数字越大，优先级越高
    xTaskCreate(task2, "task2", 4096, NULL, 2, NULL);
    xTaskCreate(task1, "task1", 4096, NULL, 1, NULL);
    // vTaskStartScheduler();
#endif
// 可以通过函数在任务运行过程中修改任务优先级
#ifdef task_other_state
    TaskHandle_t task_suspend_handle;
    //  xTaskCreate(task_suspend, "suspend", 4096, NULL, 2, &task_suspend_handle);
    xTaskCreate(task_suspend_utill, "suspend_until", 4096, NULL, 2, NULL);
    // vTaskSuspend(task_suspend_handle);  // 在外使用suspend
#endif
#ifdef task_idle_fuc
    // xTaskCreate(vApplicationIdleHook,'')
    TaskHandle_t task_idleprintf_handle;
    xTaskCreate(task_idlehook_printf, "task_idle", 4096, NULL, 2, &task_idleprintf_handle);
    //  vTaskPrioritySet(task_idleprintf_handle,1);//修改优先级
    // esp_log_set_vprintf( esp_apptrace_vprintf ) ;
    //  esp_log_set_vprintf( vprintf );

    /* config down buffer */
    char      buf[] = "Hello World!";
    esp_err_t res   = esp_apptrace_write(ESP_APPTRACE_DEST_TRAX, buf, strlen(buf), ESP_APPTRACE_TMO_INFINITE);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write data to host!");
        //  return res;
    }
    //  vTaskSetIdleTaskHookFunction( vApplicationIdleHook );
#endif
#ifdef task_delete_
    static const char* pv_task_delete = "task delete is running";
    TaskHandle_t       task_delete_handle;
    xTaskCreate(task_delete, "task_delete", 4096, (void*)pv_task_delete, 2, &task_delete_handle);
    vTaskDelete(task_delete_handle);
#endif
#ifdef resource_critical  // 临界区
    static const char* task_cirtical_ = "task delete is running";
    xTaskCreate(task_cirtical_suspend, "task_cirtical_one", 4096, (void*)task_cirtical_, 2, NULL);
// xTaskCreate(task_cirtical,"task_cirtical_two",4096,(void*)task_cirtical,2,NULL);
#endif
#ifdef resource_mutex
    xMutex = xSemaphoreCreateMutex();
    if (xMutex != NULL) {
        static const char* task_mutex_1 = "--------------\r\n";
        static const char* task_mutex_2 = "**************\r\n";
        xTaskCreate(prvPrintTask, "taskMutex1", 4096, (void*)task_mutex_1, 1, NULL);
        //"--------------\r\n"
        xTaskCreate(prvPrintTask, "taskMutex2", 4096, (void*)task_mutex_2, 2, NULL);
        //"**************\r\n"
        // vTaskStartScheduler();
    }
#endif
#ifdef resource_gate_keeper
#endif
#ifdef queue_test
    QueueHandle_t Que_testSent;
    Que_testSent = xQueueCreate(10, sizeof(testQueue));
    // Que_testRecive=xQueueCreate(10,sizeof(testQueue));
    testQueue Que_test, Que_rec;
    Que_test.a = 10;
    Que_test.b = 'J';
    if (Que_testSent != 0) {
        xQueueSend(Que_testSent, &Que_test, (TickType_t*)0);
    }
    if (Que_testSent != 0) {
        xQueueReceive(Que_testSent, &Que_rec, (TickType_t*)0);
        ESP_LOGI(TAG, "%d", Que_rec.a);
        ESP_LOGI(TAG, "%c", Que_rec.b);
    }
// 在main函数中的不同位置进行发送
#endif
//#ifdef softTimer
//#endif
//#ifdef task_delay
#if defined(USE_TIMER_TEST)&&(USE_TIMER_TEST==1)
//xTimerCreate("timer", 100, pdTRUE, NULL, timer_callback);
#endif
}