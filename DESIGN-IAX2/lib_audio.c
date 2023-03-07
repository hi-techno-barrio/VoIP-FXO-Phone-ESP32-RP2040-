#include "audio.h"

void AudioProcessing::filter(uint8_t *input, uint8_t *output, int inputSize, int outputSize) {
  // TODO: Implement audio filtering algorithm
}

void AudioProcessing::amplify(uint8_t *input, uint8_t *output, int inputSize, int outputSize, float gain) {
  for (int i = 0; i < inputSize; i++) {
    output[i] = input[i] * gain;
  }
}

void AudioProcessing::reduceNoise(uint8_t *input, uint8_t *output, int inputSize, int outputSize) {
  for (int i = 0; i < inputSize; i++) {
    int16_t sample = ((int16_t)input[i*2]) | (((int16_t)input[i*2+1]) << 8);
    if (sample < 0) {
      sample = -sample;
    }
    output[i*2] = sample & 0xFF;
    output[i*2+1] = (sample >> 8) & 0xFF;
  }
}
