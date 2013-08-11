all:	avrio

MEGA=atmega644 #atmega644 #atmega1284

CC=avr-gcc
#CC=/usr/local/avr/bin/avr-gcc # 4.8.1 custom build
LD=avr-gcc
CFLAGS=-g -Os -mmcu=${MEGA} -I/usr/local/include

OBJS=main.o lcd_drv.o mc_sabertooth.o uart.o TWI_slave.o

clean:
	rm -f ${OBJS}
	rm -f avrio.elf
	rm -f avrio.hex
	rm -f avrio.map
	rm -f *~*
	rm -f mc_sabertooth

avrio:	${OBJS}
	avr-gcc -g -mmcu=${MEGA} -o avrio.elf ${OBJS}
	avr-objcopy -j .text -j .data -O ihex avrio.elf avrio.hex

deploy:
	#sudo avrdude -p ${MEGA} -c dragon_isp -P usb -U avrio.hex
	sudo avrdude -p ${MEGA} -c avrispmkII -P usb -U avrio.hex
