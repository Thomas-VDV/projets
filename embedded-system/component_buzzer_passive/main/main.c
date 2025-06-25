#include "driver/gpio.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "rom/ets_sys.h"
#include "sdkconfig.h"
#include <stdio.h>

#include "buzzer_passive.h"
#include "hedwig_theme.h"

void task_buzzer_passive (void *pvParameter) {
  buzzer_init(22);
  while (1) {
    int tempo = 100;
    int len = sizeof(hedwig_theme) / sizeof(melody_note_t);
    buzzer_play_melody(hedwig_theme, len, tempo);
    vTaskDelay(2000);
  }
}

void app_main () {
  nvs_flash_init ();
  vTaskDelay (2000);
  xTaskCreate (&task_buzzer_passive, "buzzer_passive_task", 2048, NULL, 5,
               NULL);
}
