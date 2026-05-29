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
  //printf("Hola mundo!");
  ADC0<1>  mics({0},16000);
  //uint16_t buffer[64]{0};

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
      
      for (uint8_t i = 0; i < 8; i++) {
        if (i < level) {
          // LED on
          pixels[i*3 + 0] = 0;    // Green
          pixels[i*3 + 1] = 30;  // Red
          pixels[i*3 + 2] = 0;    // Blue

        } else if (i == peak_level) {
          pixels[i*3 + 0] = 30;    // Green
          pixels[i*3 + 1] = 0;  // Red
          pixels[i*3 + 2] = 0;    // Blue

        } else {
         // LED off
          pixels[i*3 + 0] = 0;    // Green
          pixels[i*3 + 1] = 0;  // Red
          pixels[i*3 + 2] = 0;    // Blue
        }

        ws2812_sendarray(pixels, 24);
      }

      printf("%u\n", amplitude);

      refresh_leds_flag = false;
    }

    if(peak_decay_flag){
      if(peak_level > 0) peak_level--;
      peak_decay_flag = false;
    }
  }


  //mics.read(0, buffer, 64);

  //for(size_t i=0; i<64; i++){
  //  printf("%u\n", buffer[i]);
  //}

  for(;;);
  return 0;
}

