#include <avr/io.h>
#include <util/delay.h>
#include <cstddef>
#include "USART0.hpp"
#include "ADC0.hpp"
#include "light_ws2812.h"


int main(){
  {
    //USART0 serial(57600UL);
    //printf("Hola mundo!");
    ADC0<2>  mics;
    
    // 8 LEDs x 3 bytes (GRB order)
    uint8_t pixels[24];

    // fill all LEDs with red (G=0, R=255, B=0)
    for (uint8_t i = 0; i < 8; i++) {
        pixels[i*3 + 0] = 0;    // Green
        pixels[i*3 + 1] = 100;  // Red
        pixels[i*3 + 2] = 0;    // Blue
    }

    ws2812_sendarray(pixels, 24);
  }
  for(;;);
  return 0;
}

