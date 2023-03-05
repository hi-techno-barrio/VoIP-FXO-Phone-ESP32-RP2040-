#ifndef AUDIO_H
#define AUDIO_H

#include <driver/dac.h>
#include <driver/adc.h>
#include <driver/ledc.h>
#include <driver/gpio.h>
#include <cstring>
#include "esp_adc_cal.h"

void init_audio();
void read_audio_buffer();
void connect_audio_output();
void disconnect_audio_output();
void init_adc();
int get_audio_volume();

#endif
