#include <avr/io.h>
#include <util/delay.h>
#include <cstddef>
#include <cstdlib>
#include "USART0.hpp"
#include "ADC0.hpp"
#include "light_ws2812.h"

// AVR's minimal C++ runtime does not include operator delete.
// Virtual destructors internally reference it, so we provide
// empty stubs to satisfy the linker.
void operator delete(void*, unsigned int) {}
void operator delete(void*) {}

ADCBase* adc_instance = nullptr;
volatile bool refresh_leds_flag = false;
volatile bool peak_decay_flag = false;

int main(){
  USART0 serial(57600UL);
  ADC0<1>  mics({0},8000);

  if (adc_instance) printf("instance ok\r\n");
  else printf("instance null!\r\n");
    
  // 8 LEDs x 3 bytes (GRB order)
  uint8_t pixels[24];

  // turn off  all LEDs (G=0, R=0, B=0)
  for (uint8_t i = 0; i < 8; i++) {
      pixels[i*3 + 0] = 0;    // Green
      pixels[i*3 + 1] = 0;  // Red
      pixels[i*3 + 2] = 0;    // Blue
  }
  ws2812_sendarray(pixels, 24);

  printf("inicio\n");

  mics.take(0); //continuous sampling

  uint8_t peak_level = 0;

  for(;;){
    if(refresh_leds_flag){
      //adc value 0-1023 //channel 0 in this case
      uint16_t sample = mics.peek_last(0);
      //mic has offset voltage of 1.5 v. subtract it
      int16_t centered = (int16_t)sample - 256;
      uint16_t amplitude = abs(centered);
      uint8_t level = amplitude >> 5;  // divide by 32, gives 0-7

      if (level > peak_level) {
        peak_level = level;
      }
      
      // GRB gradient per position: green (low) -> yellow (mid) -> red (high)
      static const uint8_t LEVEL_COLORS[8][3] = {
          {30,  0, 0},
          {30,  0, 0},
          {30, 10, 0},
          {20, 20, 0},
          {20, 20, 0},
          {10, 30, 0},
          { 0, 30, 0},
          { 0, 30, 0},
      };

      for (uint8_t i = 0; i < 8; i++) {
        if (i < level) {
          pixels[i*3 + 0] = LEVEL_COLORS[i][0];
          pixels[i*3 + 1] = LEVEL_COLORS[i][1];
          pixels[i*3 + 2] = LEVEL_COLORS[i][2];
        } else if (i == peak_level) {
          pixels[i*3 + 0] = 10;  // white-ish peak marker
          pixels[i*3 + 1] = 10;
          pixels[i*3 + 2] = 10;
        } else {
          pixels[i*3 + 0] = 0;
          pixels[i*3 + 1] = 0;
          pixels[i*3 + 2] = 0;
        }
      }
      ws2812_sendarray(pixels, 24);

      printf("%u\n", amplitude);

      refresh_leds_flag = false;
    }

    if(peak_decay_flag){
      if(peak_level > 0) peak_level--;
      peak_decay_flag = false;
    }
  }

  return 0;
}

