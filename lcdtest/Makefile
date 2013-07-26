all:	demo

MEGA=atmega644 #atmega644 #atmega1284

CC=avr-gcc
#CC=/usr/local/avr/bin/avr-gcc # 4.8.1 custom build
LD=avr-gcc
CFLAGS=-g -Os -mmcu=${MEGA} -I/usr/local/include

OBJS=main.o lcd_drv.o

# avr-gcc -g -Os -mmcu=atmega644 -c demo.c
# avr-gcc -g -mmcu=atmega644 -o demo.elf demo.o
# avr-objcopy -j .text -j .data -O ihex demo.elf demo.hex
# sudo avrdude -p m644 -c avrispmkII -P usb -U demo.hex

clean:
	rm ${OBJS}
	rm demo.elf
	rm demo.hex
	rm demo.map

demo:	${OBJS}
	avr-gcc -g -mmcu=${MEGA} -o demo.elf ${OBJS}
	avr-objcopy -j .text -j .data -O ihex demo.elf demo.hex

deploy:
	#sudo avrdude -p ${MEGA} -c dragon_isp -P usb -U demo.hex
	sudo avrdude -p ${MEGA} -c avrispmkII -P usb -U demo.hex
