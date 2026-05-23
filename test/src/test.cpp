#include <avr/io.h>
#include <util/delay.h>

class ScopedLED{
  public:
    ScopedLED(volatile uint8_t &ddr, volatile uint8_t &port, uint8_t pin): ddr_(ddr), port_(port), pin_(pin){
      ddr_ |= (1 << pin_);
      port_ |= (1 << pin_);
    }

    ~ScopedLED(){
      port_ &= ~(1 << pin_);
    }
    
    // non-copyable to avoid multiple destructors touching the same registers
    ScopedLED(const ScopedLED&) = delete;
    ScopedLED& operator=(const ScopedLED&) = delete;

  private:
    volatile uint8_t &ddr_;
    volatile uint8_t &port_;
    uint8_t pin_;
};




int main() {
  {
    ScopedLED led(DDRB, PORTB, PB5);
    _delay_ms(5000);
  }
  for(;;);
  return 0;
}
