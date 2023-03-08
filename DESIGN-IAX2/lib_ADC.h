// ADC.h

#ifndef ADC_H
#define ADC_H

#include <stdint.h>

typedef enum {
    ADC_RESOLUTION_6BIT = 0,
    ADC_RESOLUTION_8BIT,
    ADC_RESOLUTION_10BIT,
    ADC_RESOLUTION_12BIT
} adc_resolution_t;

typedef enum {
    ADC_CLOCK_DIV_2 = 0,
    ADC_CLOCK_DIV_4,
    ADC_CLOCK_DIV_8,
    ADC_CLOCK_DIV_16,
    ADC_CLOCK_DIV_32,
    ADC_CLOCK_DIV_64,
    ADC_CLOCK_DIV_128
} adc_clock_div_t;

// Define the ADC reference voltage in millivolts
#define ADC_REF_VOLTAGE 3300

class ADC {
  public:
    static void begin();
    static void setResolution(uint8_t bits);
    static void setClockDivisor(uint8_t divisor);
    static void start(uint8_t channel);
    static bool isConverting();
    static bool isEnd();
    static uint16_t getResult();
    static float getVoltage();
  private:
    static uint8_t adcResolution;
    static uint8_t adcClockDivisor;
};

#endif

