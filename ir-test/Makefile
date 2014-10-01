all:	irtest

MEGA=atmega644 #atmega644 #atmega1284

CC=avr-gcc
#CC=/usr/local/avr/bin/avr-gcc # 4.8.1 custom build
LD=avr-gcc
CFLAGS=-g -Os -mmcu=${MEGA} -I/usr/local/include

OBJS=main.o uart.o

clean:
	rm -f ${OBJS}
	rm -f irtest.elf
	rm -f irtest.hex
	rm -f irtest.map
	rm -f *~*
	rm -f mc_sabertooth

irtest:	${OBJS}
	avr-gcc -g -mmcu=${MEGA} -o irtest.elf ${OBJS}
	avr-objcopy -j .text -j .data -O ihex irtest.elf irtest.hex

deploy:
	#sudo avrdude -p ${MEGA} -c dragon_isp -P usb -U irtest.hex
	sudo avrdude -p ${MEGA} -c avrispmkII -P usb -U irtest.hex
