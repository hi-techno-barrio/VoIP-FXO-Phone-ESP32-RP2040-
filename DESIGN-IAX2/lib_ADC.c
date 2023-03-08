// ADC.cpp

#include "adc.h"

#define ADC_REF_VOLTAGE 3300

uint8_t ADC::adcResolution;

void ADC::begin() {
  // Initializes the ADC
  ADCSRA |= (1 << ADEN);
}

void ADC::setResolution(uint8_t bits) {
  // Sets the resolution of the ADC (8, 10, 12 bits)
  if (bits == 8) {
    ADMUX &= ~(1 << ADLAR);
    adcResolution = 8;
  } else if (bits == 10) {
    ADMUX &= ~(1 << ADLAR);
    adcResolution = 10;
  } else if (bits == 12) {
    ADMUX |= (1 << ADLAR);
    adcResolution = 12;
  }
}

void ADC::setClockDivisor(uint8_t divisor) {
  // Sets the clock divisor for the ADC (2, 4, 8, 16, 32, 64, 128, 256)
  if (divisor == 2) {
    ADCSRA |= (1 << ADPS0);
    ADCSRA &= ~(1 << ADPS1);
    ADCSRA &= ~(1 << ADPS2);
  } else if (divisor == 4) {
    ADCSRA &= ~(1 << ADPS0);
    ADCSRA |= (1 << ADPS1);
    ADCSRA &= ~(1 << ADPS2);
  } else if (divisor == 8) {
    ADCSRA |= (1 << ADPS0);
    ADCSRA |= (1 << ADPS1);
    ADCSRA &= ~(1 << ADPS2);
  } else if (divisor == 16) {
    ADCSRA &= ~(1 << ADPS0);
    ADCSRA &= ~(1 << ADPS1);
    ADCSRA |= (1 << ADPS2);
  } else if (divisor == 32) {
    ADCSRA |= (1 << ADPS0);
    ADCSRA &= ~(1 << ADPS1);
    ADCSRA |= (1 << ADPS2);
  } else if (divisor == 64) {
    ADCSRA &= ~(1 << ADPS0);
    ADCSRA |= (1 << ADPS1);
    ADCSRA |= (1 << ADPS2);
  } else if (divisor == 128) {
    ADCSRA |= (1 << ADPS0);
    ADCSRA |= (1 << ADPS1);
    ADCSRA |= (1 << ADPS2);
  } else if (divisor == 256) {
    ADCSRA &= ~(1 << ADPS0);
    ADCSRA &= ~(1 << ADPS1);
    ADCSRA &= ~(1 << ADPS2);
  }
}

void ADC::start(uint8_t channel) {
  // Starts a conversion on the specified channel
  ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
  ADCSRA
