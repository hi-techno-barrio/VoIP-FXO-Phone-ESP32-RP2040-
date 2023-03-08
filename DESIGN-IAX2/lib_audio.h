#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>

class AudioProcessing {
  public:
    static void filter(uint8_t *input, uint8_t *output, int inputSize, int outputSize);
    static void amplify(uint8_t *input, uint8_t *output, int inputSize, int outputSize, float gain);
    static void reduceNoise(uint8_t *input, uint8_t *output, int inputSize, int outputSize);
    static void setCodec(Codec *codec);
    static void beginRecord(uint8_t *buffer, int bufferSize);
  
  private:
    static Codec *audioCodec;
    static uint8_t *audioBuffer;
    static int audioBufferSize;
};

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
    void compress(uint8_t *input, uint8_t *output, int inputSize);
    void decompress(uint8_t *input, uint8_t *output, int outputSize);
};

class aLawCodec : public Codec {
  public:
    const char* getName() { return "aLaw"; }
    int getSampleRate() { return 8000; }
    int getBitsPerSample() { return 8; }
    int getBlockSize() { return 160; }
    void compress(uint8_t *input, uint8_t *output, int inputSize);
    void decompress(uint8_t *input, uint8_t *output, int outputSize);
};

#endif
