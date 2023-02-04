#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "pico/stdlib.h"


void task_1()
{   
    while (true) {
        printf("Hello from task 1\n");
        vTaskDelay(200);
    
    }
}

void task_2()
{   
    while (true) {
        printf("Hello from task 2\n");
        vTaskDelay(200);
    
    }
}


int main()
{
    stdio_init_all();

    xTaskCreate(task_1, "Task_1", 256, NULL, 1, NULL);
    xTaskCreate(task_2, "Task_2", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while(1){};
}