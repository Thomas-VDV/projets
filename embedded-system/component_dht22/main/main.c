#include "driver/gpio.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "rom/ets_sys.h"
#include "sdkconfig.h"
#include <stdio.h>

#include "dht22.h"

void task_DHT (void *pvParameter) {
  DHT22_t sensor = dht22_setup (4);

  while (1) {
    printf ("=== Reading DHT ===\n");
    dht22_read (&sensor);
    printf ("Humidity %.1f\n", sensor.humidity);
    printf ("Temperature %.1f\n", sensor.temperature);
    vTaskDelay (3000);
  }
}

void app_main () {
  nvs_flash_init ();
  vTaskDelay (2000);
  xTaskCreate (&task_DHT, "DHT_task", 2048, NULL, 5, NULL);
}
