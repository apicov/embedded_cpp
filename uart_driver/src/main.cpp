#include <avr/io.h>
#include <util/delay.h>
#include <cstddef>
#include "USART0.hpp"
#include "ADC0.hpp"
#include "light_ws2812.h"

// AVR's minimal C++ runtime does not include operator delete.
// Virtual destructors internally reference it, so we provide
// empty stubs to satisfy the linker.
void operator delete(void*, unsigned int) {}
void operator delete(void*) {}

ADCBase* adc_instance = nullptr;

int main(){
  USART0 serial(57600UL);
  //printf("Hola mundo!");
  ADC0<1>  mics({0},16000);
  uint16_t buffer[64]{0};

  if (adc_instance) printf("instance ok\r\n");
  else printf("instance null!\r\n");
    
  // 8 LEDs x 3 bytes (GRB order)
  uint8_t pixels[24];

  // fill all LEDs with red (G=0, R=255, B=0)
  for (uint8_t i = 0; i < 8; i++) {
      pixels[i*3 + 0] = 0;    // Green
      pixels[i*3 + 1] = 30;  // Red
      pixels[i*3 + 2] = 0;    // Blue
  }

  ws2812_sendarray(pixels, 24);

  printf("inicio\n");

  mics.take(64);
  printf("tomando datos\n");

  while(!mics.is_ready());

  printf("ready\n");

  mics.read(0, buffer, 64);

  for(size_t i=0; i<64; i++){
    printf("%u\n", buffer[i]);
  }

  for(;;);
  return 0;
}

