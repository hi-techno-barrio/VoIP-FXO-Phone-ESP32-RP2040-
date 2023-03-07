#include "audio.h"
#include <math.h>


void AudioProcessing::filter(uint8_t *input, uint8_t *output, int inputSize, int outputSize) {
  // Implement a simple low-pass filter
  float alpha = 0.2;
  int16_t lastSample = 0;
  
  for (int i = 0; i < inputSize; i += 2) {
    int16_t sample = ((int16_t)input[i]) | (((int16_t)input[i+1]) << 8);
    sample = (alpha * sample) + ((1 - alpha) * lastSample);
    lastSample = sample;
    output[i] = sample & 0xFF;
    output[i+1] = (sample >> 8) & 0xFF;
  }
}


void AudioProcessing::amplify(uint8_t *input, uint8_t *output, int inputSize, int outputSize, float gain) {
  // Amplify the audio data
  for (int i = 0; i < inputSize; i++) {
    int16_t sample = ((int16_t)input[i*2]) | (((int16_t)input[i*2+1]) << 8);
    sample *= gain;
    if (sample > 32767) {
      sample = 32767;
    } else if (sample < -32768) {
      sample = -32768;
    }
    output[i*2] = sample & 0xFF;
    output[i*2+1] = (sample >> 8) & 0xFF;
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


class Codec {
  public:
    virtual const char* getName() = 0;
    virtual int getSampleRate() = 0;
    virtual int getBitsPerSample() = 0;
    virtual int getBlockSize() = 0;
    virtual void compress(uint8_t *input, uint8_t *output, int inputSize) = 0;
    virtual void decompress(uint8_t *input, uint8_t *output, int outputSize) = 0;
};

class uLawCodec : public Codec {
  public:
    const char* getName() { return "uLaw"; }
    int getSampleRate() { return 8000; }
    int getBitsPerSample() { return 8; }
    int getBlockSize() { return 160; }

    void compress(uint8_t *input, uint8_t *output, int inputSize) {
      for (int i = 0; i < inputSize; i++) {
        int16_t sample = ((int16_t)input[i*2]) | (((int16_t)input[i*2+1]) << 8);
        sample = sample >> 2;
        sample = sample ^ 0x8000;
        if (sample < 0) {
          sample = -sample;
          output[i] = 0xFF ^ ((int)log2(sample) >> 3);
        } else {
          output[i] = 0x7F ^ ((int)log2(sample) >> 3);
        }
      }
    }

    void decompress(uint8_t *input, uint8_t *output, int outputSize) {
      for (int i = 0; i < outputSize; i++) {
        int16_t sample = 0;
        if (input[i] & 0x80) {
          sample = (1 << ((input[i] & 0x0F) + 3)) ^ 0x8000;
        } else {
          sample = ((input[i] & 0x0F) << 3) | 0x84;
        }
        output[i*2] = sample & 0xFF;
        output[i*2+1] = (sample >> 8) & 0xFF;
      }
    }
};

class aLawCodec : public Codec {
  public:
    const char* getName() { return"aLaw"; }
int getSampleRate() { return 8000; }
int getBitsPerSample() { return 8; }
int getBlockSize() { return 160; }
 void compress(uint8_t *input, uint8_t *output, int inputSize) {
  for (int i = 0; i < inputSize; i++) {
    int16_t sample = ((int16_t)input[i*2]) | (((int16_t)input[i*2+1]) << 8);
    sample = sample >> 3;
    int sign = (sample >> 8) & 0x80;
    if (sign) {
      sample = -sample;
    }
    if (sample > 32635) {
      sample = 32635;
    }
    if (sample >= 256) {
      int exponent = 7 - (int)log2(sample);
      int mantissa = (sample >> (exponent + 3)) & 0x0F;
      output[i] = sign | (exponent << 4) | mantissa;
    } else {
      output[i] = sign | (sample >> 4);
    }
    output[i] ^= 0x55;
  }
}

void decompress(uint8_t *input, uint8_t *output, int outputSize) {
  for (int i = 0; i < outputSize; i++) {
    int sign = input[i] & 0x80;
    int exponent = (input[i] >> 4) & 0x07;
    int mantissa = input[i] & 0x0F;
    int sample = (mantissa << (exponent + 3));
    sample |= 1 << (exponent + 2);
    if (sign) {
      sample = -sample;
    }
    output[i*2] = sample & 0xFF;
    output[i*2+1] = (sample >> 8) & 0xFF;
  }
}
 };
