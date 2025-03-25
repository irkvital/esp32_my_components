#include "button_fix_bounce.h"

static QueueHandle_t button_queue = NULL;

void initPinInp(int pin) {
    printf("%d\n", pin);
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    gpio_set_level(pin, 1);
}

// обработчик по нажатию на кнопку
static void IRAM_ATTR buttonHandle(void* gpio_num)
{
    // Переменные для переключения контекста
    BaseType_t xHigherPriorityTaskWoken, xResult;
    xHigherPriorityTaskWoken = pdFALSE;
    bool pressed;
    if (gpio_get_level((gpio_num_t) gpio_num) == 0) {
        pressed = true;
    } else {
        pressed = false;
    }
    // Отправляем в очередь задачи событие "кнопка нажата"
    xResult = xQueueSendFromISR(button_queue, &pressed, &xHigherPriorityTaskWoken);
    // Если высокоприоритетная задача ждет этого события, переключаем управление
    if (xResult == pdPASS) {
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void taskButton(void* arg) {
    bool state = false;
    bool last_button_state = false;
    bool tmp = false;
    TickType_t prew = xTaskGetTickCount();
    TickType_t curr;
    // время ожидания дребезга контактов в мс
    TickType_t wait_tick = 50;
    wait_tick /= portTICK_PERIOD_MS;
    while (1) {
        if (xQueueReceive(button_queue, &state, portMAX_DELAY)) {
            curr = xTaskGetTickCount();
            // проверка на дребезг контактов (на событие нажатия)
            if (state == true && last_button_state == false && (curr - prew) > (wait_tick)) {
                last_button_state = true;
                buttonFixPressed();
            // проверка на дребезг контактов (на сыбытие отжатия)
            } else if (state == false && last_button_state == true && !xQueuePeek(button_queue, &tmp, wait_tick)) {
                last_button_state = false;
                buttonFixReleased();
            }
            prew = curr;
        }
    }
    vTaskDelete(NULL);
}

void buttonFixInit(int gpio_num) {
    if (button_queue == NULL) {
        button_queue = xQueueCreate(30, sizeof(bool));
    }

    initPinInp(gpio_num);
    // Устанавливаем сервис GPIO ISR service
    esp_err_t err = gpio_install_isr_service(0);
    if (err == ESP_ERR_INVALID_STATE) {
        ESP_LOGW("ISR", "GPIO isr service already installed");
    };

    // Регистрируем обработчик прерывания на нажатие кнопки
    gpio_isr_handler_add(gpio_num, buttonHandle, (void*) gpio_num);
    gpio_set_intr_type(gpio_num, GPIO_INTR_ANYEDGE);
    // Разрешаем использование прерываний
    gpio_intr_enable(gpio_num);

    xTaskCreatePinnedToCore(taskButton, "taskButton", 3072, (void*) gpio_num, 20, NULL, tskNO_AFFINITY);
}
 
void app_main(void)
{
    buttonFixInit(GPIO_NUM_19);
}

void buttonFixPressed() {
    ESP_LOGI("MAIN", "Button is pressed");
}

void buttonFixReleased() {
    ESP_LOGI("MAIN", "Button is released\n");
}