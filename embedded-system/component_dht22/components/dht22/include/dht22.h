#ifndef DHT22_H
#define DHT22_H

#include <driver/gpio.h>
#include <stdbool.h>

typedef struct {
  gpio_num_t gpio;
  float temperature;
  float humidity;
} DHT22_t;

DHT22_t dht22_setup (int gpio);
bool dht22_read (DHT22_t *sensor);

#endif // DHT22_H
