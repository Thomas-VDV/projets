#include "buzzer_passive.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>

#define BUZZER_TIMER LEDC_TIMER_0
#define BUZZER_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_CHANNEL LEDC_CHANNEL_0
#define DUTY_RESOLUTION LEDC_TIMER_13_BIT
#define DUTY_CYCLE ((1 << DUTY_RESOLUTION) / 2)

static const char *TAG = "buzzer";
static int buzzer_gpio = -1;

static const float note_freqs[] = {
    261.63, 277.18, 293.66, 311.13, 329.63,
    349.23, 369.99, 392.00, 415.30, 440.00,
    466.16, 493.88, 0.0 // NOTE_REST
};

static int get_duration_ms(note_type_t type, int tempo_bpm) {
  float beat = 60000.0f / tempo_bpm;
  switch (type) {
    case NOTE_RONDE: return 4 * beat;
    case NOTE_BLANCHE: return 2 * beat;
    case NOTE_NOIR_POINT: return 1.5 * beat;
    case NOTE_NOIR: return beat;
    case NOTE_CROCHE_POINT: return 0.75 * beat;
    case NOTE_CROCHE: return beat / 2;
    case NOTE_DOUBLE_CROCHE: return beat / 4;
    default: return beat;
  }
}

esp_err_t buzzer_init(int gpio_num) {
  buzzer_gpio = gpio_num;

  ledc_timer_config_t timer_conf = {
    .speed_mode = BUZZER_MODE,
    .timer_num = BUZZER_TIMER,
    .duty_resolution = DUTY_RESOLUTION,
    .freq_hz = 1000, 
    .clk_cfg = LEDC_AUTO_CLK
  };

  esp_err_t err = ledc_timer_config(&timer_conf);
  if (err != ESP_OK) return err;

  ledc_channel_config_t channel_conf = {
    .channel = BUZZER_CHANNEL,
    .duty = 0,
    .gpio_num = buzzer_gpio,
    .speed_mode = BUZZER_MODE,
    .hpoint = 0,
    .timer_sel = BUZZER_TIMER
  };

  return ledc_channel_config(&channel_conf);
}

esp_err_t buzzer_play_note(note_name_t note, int octave, int duration_ms) {
  if (note == NOTE_REST) {
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    return ESP_OK;
  }

  float base_freq = note_freqs[note];
  int freq = (int)(base_freq * powf(2, octave - 4));

  esp_err_t err = ledc_set_freq(BUZZER_MODE, BUZZER_TIMER, freq);
  if (err != ESP_OK) return err;

  ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, DUTY_CYCLE);
  ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);

  vTaskDelay(pdMS_TO_TICKS(duration_ms));

  ledc_set_duty(BUZZER_MODE, BUZZER_CHANNEL, 0);
  ledc_update_duty(BUZZER_MODE, BUZZER_CHANNEL);

  return ESP_OK;
}

esp_err_t buzzer_play_melody(const melody_note_t *melody, int length, int tempo_bpm) {
  for (int i = 0; i < length; ++i) {
    int duration = get_duration_ms(melody[i].type, tempo_bpm);
    buzzer_play_note(melody[i].note, melody[i].octave, duration);
    vTaskDelay(pdMS_TO_TICKS(duration / 10));//silence between note
  }
  return ESP_OK;
}
