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
    const char* getName() { return "aLaw"; }
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
class G711Codec : public Codec {
public:
const char* getName() { return "G.711"; }
int getSampleRate() { return 8000; }
int getBitsPerSample() { return 8; }
int getBlockSize() { return 160; }
void compress(uint8_t *input, uint8_t *output, int inputSize) {
  for (int i = 0; i < inputSize; i++) {
    int16_t sample = ((int16_t)input[i*2]) | (((int16_t)input[i*2+1]) << 8);
    output[i] = linear2alaw(sample);
  }
}

void decompress(uint8_t *input, uint8_t *output, int outputSize) {
  for (int i = 0; i < outputSize; i++) {
    int16_t sample = alaw2linear(input[i]);
    output[i*2] = sample & 0xFF;
    output[i*2+1] = (sample >> 8) & 0xFF;
  }
}
private:
uint8_t linear2alaw(int16_t pcm_val) {
const int16_t ALAW_MAX = 0xFFF;
const int16_t ALAW_BIAS = 0x84;
int16_t mask;
uint8_t aval;  if (pcm_val >= 0) {
    mask = 0xD5;
  } else {
    mask = 0x55;
    pcm_val = -pcm_val - 8;
  }

  if (pcm_val > ALAW_MAX) {
    pcm_val = ALAW_MAX;
  }

  pcm_val += ALAW_BIAS;

  if (pcm_val > 0x7F) {
    aval = (uint8_t)(~mask | (pcm_val >> 1));
  } else {
    aval = (uint8_t)(mask | (pcm_val >> 1));
  }

  return aval;
}

int16_t alaw2linear(uint8_t a_val) {
  const int16_t ALAW_BIAS = 0x84;
  int16_t t, seg;
  a_val ^= 0x55;
  t = (a_val & 0x7F) << 4;
  seg = ((unsigned)a_val & 0x70) >> 4;
  switch (seg) {
    case 0:
      t += 8;
      break;
    case 1:
      t += 0x108;
      break;
    default:
      t += 0x108;
      t <<= seg - 1;
  }
  return ((a_val & 0x80) ? t : -t) - ALAW_BIAS;
}
};

class uLawG711Codec : public Codec {
public:
const char* getName() { return "uLaw-G.711"; }
int getSampleRate() { return 8000; }
int getBitsPerSample() { return 8; }
int getBlockSize() { return 160; }void compress(uint8_t *input, uint8_t *output, int inputSize) {
  uint8_t ulawInput[inputSize];
  uLawCodec ulaw;
  ulaw.compress(input, ulawInput, inputSize);

  G711Codec g711;
  g711.compress(ulawInput, output, inputSize);
}

void decompress(uint8_t *input, uint8_t *output, int outputSize) {
  uint8_t g711Input[outputSize];
  G711Codec g711
class G711Codec : public Codec {
public:
const char* getName() { return "G.711"; }
int getSampleRate() { return 8000; }
int getBitsPerSample() { return 8; }
int getBlockSize() { return 160; }void compress(uint8_t *input, uint8_t *output, int inputSize) {
  for (int i = 0; i < inputSize; i++) {
    int16_t sample = ((int16_t)input[i*2]) | (((int16_t)input[i*2+1]) << 8);
    int8_t encodedSample = linearToMuLaw(sample);
    output[i] = (uint8_t)encodedSample;
  }
}

void decompress(uint8_t *input, uint8_t *output, int outputSize) {
  for (int i = 0; i < outputSize; i++) {
    int8_t encodedSample = (int8_t)input[i];
    int16_t sample = muLawToLinear(encodedSample);
    output[i*2] = sample & 0xFF;
    output[i*2+1] = (sample >> 8) & 0xFF;
  }
}
private:
static int8_t linearToMuLaw(int16_t sample) {
const int BIAS = 132;
const int CLIP = 32635;
const int MULAW_MAX = 0x1F;
int sign = (sample >> 8) & 0x80;
if (sign) {
sample = ~sample;
}
if (sample > CLIP) {
sample = CLIP;
}
sample += BIAS;
int exponent = log2((sample >> 7) & 0xFF);
int mantissa = (sample >> (exponent + 3)) & 0x0F;
int encodedSample = ~(sign | (exponent << 4) | mantissa);
return (int8_t)(encodedSample & MULAW_MAX);
}

static int16_t muLawToLinear(int8_t encodedSample) {
  const int BIAS = 132;
  const int MULAW_MAX = 0x1F;
  int sign = encodedSample & 0x80;
  int exponent = (encodedSample >> 4) & 0x07;
  int mantissa = encodedSample & 0x0F;
  int decodedSample = ((mantissa << 3) | 0x84) << (exponent - 1);
  decodedSample += BIAS;
  if (sign) {
    decodedSample = -decodedSample;
  }
  return (int16_t)decodedSample;
}
};

#endif // AUDIO_H
