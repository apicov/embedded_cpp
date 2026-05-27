#pragma once

#include <avr/io.h>
#include <cstdio>

class USART0;
static USART0* uart_instance = nullptr;

//UBRR0 — baud rate register, set it to (F_CPU / (16UL * baud)) - 1
//UCSR0B — enable TX with (1 << TXEN0), RX with (1 << RXEN0)
//UCSR0C — set frame format (8 data bits, 1 stop bit)
//UDR0 — data register, write a byte to transmit, read a byte to receive

int uart_putchar_cb(char c, FILE* f);

class USART0{
  public:
    USART0(uint32_t baudrate): baudrate_(baudrate), prev_ubrr0_(UBRR0), prev_uscr0c_(UCSR0C) {
      // set uart as std output
      uart_instance = this;
      static FILE uart_stdout;
      fdev_setup_stream(&uart_stdout, uart_putchar_cb, nullptr, _FDEV_SETUP_WRITE);
      stdout = &uart_stdout;

      UBRR0 =  (F_CPU / (16UL * baudrate_)) - 1; 
      UCSR0B |= (1 << TXEN0);
      UCSR0B |= (1 << RXEN0);
      UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data  bits 1 stop bit   
    }

    ~USART0(){
      while (!(UCSR0A & (1 << TXC0)));  // wait until data transmited  
      UBRR0 = prev_ubrr0_;
      UCSR0C = prev_uscr0c_;
      UCSR0B &= ~(1 << TXEN0);
      UCSR0B &= ~(1 << RXEN0);
    }

    void put_char(char c){
      //UCSR0A — the UART status register. UDRE0 (UART Data Register Empty).
      while (!(UCSR0A & (1 << UDRE0)));  // wait until ready  
      UDR0 = c; //send
    }

    void put_str(const char* str){
      size_t cnt = 0;
      while(str[cnt] != '\0'){
        put_char(str[cnt]);
        cnt++;
       }
      while (!(UCSR0A & (1 << UDRE0)));  // wait until ready  
    }

  private:
    uint32_t baudrate_;
    uint16_t prev_ubrr0_;
    uint8_t prev_uscr0c_;
};

int uart_putchar_cb(char c, FILE* f) {
    (void)f;
    uart_instance->put_char(c);
    return 0;
}


