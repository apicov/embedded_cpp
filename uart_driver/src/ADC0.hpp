#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <array>
#include "CircularBuffer.hpp"


class ADCBase {
public:
    virtual size_t num_ch() const = 0;
    uint8_t current_channel_idx = 0;
    virtual void store_sample(uint16_t sample) = 0;
    virtual uint8_t channel(uint8_t idx) const = 0;
    virtual ~ADCBase() {}
};

static ADCBase* adc_instance = nullptr;


template<size_t N>
class ADC0: public ADCBase {
  public:
    // public for ISR access — intentional
    static constexpr size_t num_channels = N;
    std::array<uint8_t, N> channels_;
    std::array<CircularBuffer<uint16_t, 64>, N> buffers_;

    ADC0(std::array<uint8_t, N> channels, unsigned int sample_rate ): channels_(channels){
    adc_instance = this;
    // Timer1 CTC mode: counts to OCR1A then resets, firing an interrupt
    // OCR1A sets the compare match value to hit the desired sample rate
    OCR1A  = (F_CPU / (8UL * sample_rate)) - 1;
    // WGM12: CTC mode | CS11: prescaler 8 (16MHz/8 = 2MHz timer clock)
    TCCR1B = (1 << WGM12);
    // REFS0: use AVCC as reference voltage
    // lower 4 bits: select ADC channel
    ADMUX  = (1 << REFS0) | (channels_[0] & 0x0F);
    // ADIE: enable ADC conversion complete interrupt
    // ADPS2|ADPS1|ADPS0: prescaler 128 (16MHz/128 = 125kHz ADC clock)
    ADCSRA = (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    }

    void start(){
      // OCIE1A:  enable Timer1 compare match A interrupt
      TIMSK1 |= (1 << OCIE1A);
      // enable ADC 
      ADCSRA |= (1 << ADEN);
      // start timer clock
      TCCR1B |= (1 << CS11);
      sei(); // enable global interrupts
    }

    void stop(){
      // OCIE1A:  disable Timer1 compare match A interrupt
      TIMSK1 &= ~(1 << OCIE1A);
      // disable ADC 
      ADCSRA &= ~(1 << ADEN);
      // stop timer clock
      TCCR1B &= ~(1 << CS11);
    }

    size_t num_ch() const override{
      return N;
    }

    uint8_t channel(uint8_t idx) const override{
      return channels_[idx];
    } 

    size_t read(uint8_t channel, uint16_t* output, size_t length) {
        return buffers_[channel].pop(output, length);
    }

    void store_sample(uint16_t sample) override{
      // Store sample in the buffer for the current channel
      buffers_[current_channel_idx].push(&sample, 1);

      // Move to next channel
      current_channel_idx = (current_channel_idx + 1) % N;

      // Switch ADMUX to next channel for the next conversion
      ADMUX = (1 << REFS0) | (channels_[current_channel_idx] & 0x0F);
    }

    ~ADC0() override {
      stop();
      TCCR1B = 0; //resets timer mode
      ADMUX = 0; // resets channel selection
      adc_instance = nullptr;
      cli();
    }
  private:
    uint8_t current_channel_;
};

