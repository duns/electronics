# 
# Skeleton for developing software for the Arduino Mega.  This
# Makefile seems to work well for my Arduino Mega connected to my
# Macbook.
#
# Requirements:
#
#   * on OSX you probably need to install the USB driver that comes
#     with the Arduino IDE
#   * make sure you have avr-gcc, avr-binutils, avr-libc and avrdude installed
#
#  -- Bjorn Borud <borud@borud.org>

### The name of your program and the main file
#
PROGRAM = helmet_firmware

### Object files that are part of your program
#
OBJECTS = $(PROGRAM).o \
	  tiny/WInterrupts.o \
	  tiny/wiring_analog.o \
	  tiny/wiring.o \
	  tiny/wiring_digital.o \
	  tiny/pins_arduino.o \
	  tiny/main.o \


### The dype of device
#
DEVICE = attiny84

### Clock speed of the device
#
CLOCK = 1000000

### Serial port we use for programming the Arduino
#
PORT = /dev/tty.usbserial-A9007Laf
RATE = 57600
PROTO = stk500v1

######################################################################

CC = avr-gcc
CC = /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-gcc
CXX = /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr/bin/avr-g++
AVRDUDE = avrdude
OBJCOPY = avr-objcopy
AVRSIZE = avr-size
CFLAGS = -Wall -Os -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -I ./tiny
RM = rm -f
FLASH = -U flash:w:$(PROGRAM).hex:i
AVRDUDE_FLAGS = -p$(DEVICE) -c$(PROTO) -P$(PORT) -b$(RATE) $(FLASH)

all:	$(PROGRAM).hex

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

.S.o:
	$(CC) $(CFLAGS) -x assembler-with-cpp -c $< $@

.c.s:
	$(CC) $(CFLAGS) -S $< -o $@

$(PROGRAM).elf: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(PROGRAM).elf $(OBJECTS) -lm

$(PROGRAM).hex: $(PROGRAM).elf
	$(RM) $(PROGRAM).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(PROGRAM).elf $(PROGRAM).hex
	$(AVRSIZE) *.hex *.elf *.o

upload: $(PROGRAM).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS)

disasm: $(PROGRAM).elf
	avr-objdump -d $(PROGRAM).elf

clean:
	$(RM) $(OBJECTS) $(PROGRAM).elf $(PROGRAM).hex
