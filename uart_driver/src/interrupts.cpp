#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC0.hpp"

constexpr uint16_t LED_REFRESH_COUNTS = 800;   // 20Hz
constexpr uint16_t PEAK_DECAY_COUNTS  = 1600;  // 10Hz

extern volatile bool refresh_leds_flag;
uint16_t refresh_leds_counter = 0;


extern volatile bool peak_decay_flag;
uint16_t peak_decay_counter = 0;

ISR(TIMER1_COMPA_vect) {
    // Fires at sample_rate * N Hz (e.g. 16kHz for 2 channels at 8kHz each)
    // Just trigger a new ADC conversion — result will be ready in ~13 ADC clock cycles
    ADCSRA |= (1 << ADSC); // ADSC: ADC Start Conversion
                           
    refresh_leds_counter++;
    if(refresh_leds_counter == LED_REFRESH_COUNTS){
      refresh_leds_flag = true;
      refresh_leds_counter = 0;
    }

    peak_decay_counter++;
    if(peak_decay_counter == PEAK_DECAY_COUNTS){
      peak_decay_flag = true;
      peak_decay_counter = 0;
    }

}

ISR(ADC_vect) {
    // Read the 10-bit ADC result (must read ADCL first to lock both registers)
    uint16_t sample = ADC;

    if (adc_instance) {
      adc_instance->store_sample(sample);
    }
}


