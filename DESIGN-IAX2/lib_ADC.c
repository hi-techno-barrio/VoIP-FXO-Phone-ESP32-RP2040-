// ADC.cpp

#include "ADC.h"
#include <avr/io.h>

void ADC::begin() {
    // Enable ADC and set the prescaler to 128
    ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

void ADC::setResolution(adc_resolution_t resolution) {
    // Set the ADC resolution
    ADMUX = (ADMUX & ~_BV(ADLAR)) | (resolution << ADLAR);
}

void ADC::setClockDivisor(adc_clock_div_t clockDivisor) {
    // Set the ADC clock divisor
    ADCSRA = (ADCSRA & ~0x07) | clockDivisor;
}

uint16_t ADC::read(uint8_t channel) {
    // Set the ADC channel
    ADMUX = (ADMUX & ~0x1F) | (channel & 0x1F);

    // Start the conversion
    ADCSRA |= _BV(ADSC);

    // Wait for the conversion to complete
    while (ADCSRA & _BV(ADSC)) {}

    // Return the result
    return ADC;
}
