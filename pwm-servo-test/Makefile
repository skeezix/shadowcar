all:	pwmservodemo

#ATMEGA=atmega128
ATMEGA=atmega644

CC=avr-gcc
LD=avr-gcc
#CFLAGS=-g -Os -mmcu=${ATMEGA}
CFLAGS=-g -Os -mmcu=${ATMEGA}

# avr-gcc -g -Os -mmcu=atmega644 -c demo.c
# avr-gcc -g -mmcu=atmega8 -o demo.elf demo.o
# avr-objcopy -j .text -j .data -O ihex demo.elf demo.hex
# sudo avrdude -p m644 -c avrispmkII -P usb -U demo.hex

clean:
	rm -f pwmservodemo.o
	rm -f pwmservodemo.elf
	rm -f pwmservodemo.hex
	rm -f pwmservodemo.map
	rm -f pwmservodemo
	rm -f *~*

pwmservodemo:	pwmservodemo.o
	avr-gcc -g -mmcu=${ATMEGA} -o pwmservodemo.elf pwmservodemo.o

	avr-objcopy -j .text -j .data -O ihex pwmservodemo.elf pwmservodemo.hex

deploy:
	sudo avrdude -p ${ATMEGA} -c avrispmkII -P usb -U pwmservodemo.hex
