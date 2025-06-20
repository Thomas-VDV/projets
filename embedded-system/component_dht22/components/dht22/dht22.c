#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

#include "dht22.h"

#define DHT22_START_SIGNAL_DURATION_US 3000
#define DHT22_START_SIGNAL_POST_DELAY_US 25
#define DHT22_INIT_TIMEOUT_US 85
#define DHT22_BIT_LOW_DURATION_MAX_US 56
#define DHT22_BIT_HIGH_DURATION_MAX_US 100
#define DHT22_THRESHOLD_HIGH_US 40

#define DHT22_TOTAL_BITS 40
#define DHT22_DATA_BYTES 5

#define DHT22_TEMP_VALUE_MASK 0x7F
#define DHT22_TEMP_SIGN_MASK 0x80

// Global variable
static const char *TAG = "DHT22";

// Function for wait for at a specific level
int getSignalLevel (int gpio, bool state, int usTimeOut) {

  int timing = 0;
  while (gpio_get_level (gpio) == state) {

    if (timing > usTimeOut)
      return -1;

    ++timing;
    ets_delay_us (1);
  }

  return timing;
}

// Communicates with the DHT22 sensor to retrieve data
bool dht22_read (DHT22_t *sensor) {
  int timing = 0;

  uint8_t dhtData[DHT22_TOTAL_BITS];
  for (int i = 0; i < DHT22_TOTAL_BITS; i++)
    dhtData[i] = 0;

  // Send start signal to DHT
  gpio_set_direction (sensor->gpio, GPIO_MODE_OUTPUT);

  gpio_set_level (sensor->gpio, 0);
  ets_delay_us (DHT22_START_SIGNAL_DURATION_US);

  gpio_set_level (sensor->gpio, 1);
  ets_delay_us (DHT22_START_SIGNAL_POST_DELAY_US);

  gpio_set_direction (sensor->gpio, GPIO_MODE_INPUT);

  timing = getSignalLevel (sensor->gpio, 0, DHT22_INIT_TIMEOUT_US);
  if (timing < 0) {
    ESP_LOGE (TAG, "Timeout while waiting for LOW signal on the start");
    return false;
  }

  timing = getSignalLevel (sensor->gpio, 1, DHT22_INIT_TIMEOUT_US);
  if (timing < 0) {
    ESP_LOGE (TAG, "Timeout while waiting for HIGH signal on the start");
    return false;
  }

  // Read data
  for (int i = 0; i < DHT22_TOTAL_BITS; i++) {
    timing = getSignalLevel (sensor->gpio, 0, DHT22_BIT_LOW_DURATION_MAX_US);
    if (timing < 0) {
      ESP_LOGE (TAG, "Timeout while waiting for LOW signal (step %d)", i);
      return false;
    }

    timing = getSignalLevel (sensor->gpio, 1, DHT22_BIT_HIGH_DURATION_MAX_US);
    if (timing < 0) {
      ESP_LOGE (TAG, "Timeout while waiting for HIGH signal (step %d)", i);
      return false;
    }

    if (timing > DHT22_THRESHOLD_HIGH_US) {
      dhtData[i] = 1;
    }
  }

  // Decode data
  uint8_t dataByte[DHT22_DATA_BYTES];
  for (int i = 0; i < DHT22_DATA_BYTES; i++) {
    dataByte[i] = 0;

    for (int j = 0; j < 8; j++) {
      dataByte[i] = (dataByte[i] << 1) | dhtData[i * 8 + j];
    }
  }

  // Check parity bit
  int expectParityBit =
      (dataByte[0] + dataByte[1] + dataByte[2] + dataByte[3]) & 0xFF;
  if (expectParityBit != dataByte[4]) {
    ESP_LOGE (TAG, "Checksum failed");
    return false;
  }

  sensor->humidity = dataByte[0];
  sensor->humidity *= 0x100;
  sensor->humidity += dataByte[1];
  sensor->humidity /= 10;

  sensor->temperature = dataByte[2] & DHT22_TEMP_VALUE_MASK;
  sensor->temperature *= 0x100;
  sensor->temperature += dataByte[3];
  sensor->temperature /= 10;

  if (dataByte[2] & DHT22_TEMP_SIGN_MASK) {
    sensor->temperature *= -1;
  }

  return true;
}

// Function setup the sensor
DHT22_t dht22_setup (int gpio) {
  if (gpio < 0 || gpio >= GPIO_NUM_MAX) {
    ESP_LOGE (TAG, "Invalid GPIO number: %d", gpio);
  }

  DHT22_t sensor = (DHT22_t){.gpio = gpio, .temperature = 0, .humidity = 0};
  return sensor;
}
