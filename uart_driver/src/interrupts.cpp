#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC0.hpp"

ISR(TIMER1_COMPA_vect) {
    // Fires at sample_rate * N Hz (e.g. 16kHz for 2 channels at 8kHz each)
    // Just trigger a new ADC conversion — result will be ready in ~13 ADC clock cycles
    ADCSRA |= (1 << ADSC); // ADSC: ADC Start Conversion
}

ISR(ADC_vect) {
    // Read the 10-bit ADC result (must read ADCL first to lock both registers)
    uint16_t sample = ADC;

    if (adc_instance) {
      adc_instance->store_sample(sample);
    }
}


