# make          # compile
# make flash    # compile + flash
# make clean    # remove build artifacts
# make size     # show RAM/Flash usage

TARGET = main

SRCS = $(wildcard *.cpp) $(wildcard src/*.cpp)

MCU        = atmega328p
F_CPU      = 16000000UL
STD        = c++17
PROGRAMMER = avrispmkii
PORT       = usb

CXX     = avr-g++
OBJCOPY = avr-objcopy
SIZE    = avr-size
AVRDUDE = avrdude

CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=$(STD) \
           -Os -fno-exceptions -fno-rtti \
           -Wall -Wextra \
           -ffunction-sections -fdata-sections

LDFLAGS = -mmcu=$(MCU) -Wl,--gc-sections

OBJS = $(SRCS:.cpp=.o)
ELF  = $(TARGET).elf
HEX  = $(TARGET).hex

.PHONY: all flash clean size

all: $(HEX) size

$(ELF): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

flash: $(HEX)
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -U flash:w:$(HEX)

size: $(ELF)
	$(SIZE) --mcu=$(MCU) --format=avr $(ELF)

clean:
	rm -f $(OBJS) $(ELF) $(HEX)
