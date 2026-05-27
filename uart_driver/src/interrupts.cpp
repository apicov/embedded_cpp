#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC0.hpp"

static uint8_t current_channel_idx_ = 0 ;

ISR(TIMER1_COMPA_vect) {
    // Fires at sample_rate * N Hz (e.g. 16kHz for 2 channels at 8kHz each)
    // Just trigger a new ADC conversion — result will be ready in ~13 ADC clock cycles
    ADCSRA |= (1 << ADSC); // ADSC: ADC Start Conversion
}

ISR(ADC_vect) {
    // Read the 10-bit ADC result (must read ADCL first to lock both registers)
    uint16_t sample = ADC;
    
    // Store sample in the buffer for the current channel
    adc_instance->buffers_[current_channel_idx_].push(&sample, 1);
    
    // Move to next channel
    current_channel_idx_ = (current_channel_idx_ + 1) % adc_instance->num_channels;
    
    // Switch ADMUX to next channel for the next conversion
    ADMUX = (1 << REFS0) | (adc_instance->channels_[current_channel_idx_] & 0x0F);
}


