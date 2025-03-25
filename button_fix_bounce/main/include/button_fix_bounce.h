#ifndef BUTTON_FIX_BOUNCE_H
#define BUTTON_FIX_BOUNCE_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h" 
#include "esp_log.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#define WAIT_BOUNCE_MS 50 / portTICK_PERIOD_MS
#define BUTTONS_NUM 2
#define GPIO_BUTTON_0 GPIO_NUM_19
#define GPIO_BUTTON_1 GPIO_NUM_20

static QueueHandle_t button_queue[BUTTONS_NUM];
static gpio_num_t gpio_button[BUTTONS_NUM] =   {GPIO_NUM_19,
                                                GPIO_NUM_20};

void buttonFixInit();
void buttonFixPressed(int button_num);
void buttonFixReleased(int button_num);


#endif // BUTTON_FIX_BOUNCE_H