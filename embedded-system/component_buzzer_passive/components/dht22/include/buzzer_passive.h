#ifndef BUZZER_PASSIVE_H
#define BUZZER_PASSIVE_H

#pragma once

#include "esp_err.h"

typedef enum {
  NOTE_DO, NOTE_DOD, NOTE_RE, NOTE_RED, NOTE_MI, NOTE_FA, NOTE_FAD,
  NOTE_SOL, NOTE_SOLD, NOTE_LA, NOTE_LAD, NOTE_SI, NOTE_REST
} note_name_t;

typedef enum {
  NOTE_RONDE, NOTE_BLANCHE, NOTE_NOIR_POINT, NOTE_NOIR,
  NOTE_CROCHE_POINT, NOTE_CROCHE, NOTE_DOUBLE_CROCHE
} note_type_t;

typedef struct {
  note_name_t note;
  int octave;
  note_type_t type;
} melody_note_t;

esp_err_t buzzer_init(int gpio_num);
esp_err_t buzzer_play_note(note_name_t note, int octave, int duration_ms);
esp_err_t buzzer_play_melody(const melody_note_t *melody, int length, int tempo_bpm);


#endif // BUZZER_PASSIVE_H
