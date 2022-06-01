MCU=attiny45
#F_CPU=8000000
#F_CPU=16500000
F_CPU=8000000
CC=avr-gcc
OBJCOPY=avr-objcopy
#CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I. -I../../ssd1306/src/lcd/ -I../../ssd1306/src 
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I. -I/home/beckerz/uC_Projects/ssd1306/src -I/home/beckerz/uC_Projects/ssd1306/src/lcd 
TARGET=main
SRCS=main.c

all:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

flash:
	avrdude -p ${MCU} -c stk500v1 -U flash:w:${TARGET}.hex:i -F -P /dev/ttyACM0 -b19200

fuses: 	
	avrdude -p ${MCU} -c stk500v1 -U lfuse:w:0xE2:m -P /dev/ttyACM0 -b19200

testing:
	gcc -Wall test.c -o test

clean:
	rm -f *.bin *.hex
