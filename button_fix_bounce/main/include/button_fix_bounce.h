#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h" 
#include "esp_log.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

void buttonFixInit(int gp_inp);
void buttonFixPressed();
void buttonFixReleased();