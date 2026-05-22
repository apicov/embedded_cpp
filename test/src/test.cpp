#include <avr/io.h>
#include <util/delay.h>

int main() {
    DDRB |= (1 << PB5);   // pin 13 on Uno = PB5
    for(;;) {
        PORTB ^= (1 << PB5);
        _delay_ms(500);
    }
}
