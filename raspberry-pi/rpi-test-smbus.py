#!/usr/bin/python

# see http://blog.oscarliang.net/raspberry-pi-arduino-connected-i2c/

# i2cdetect -y 1
# (-y 0 for rev1 boards)

import smbus
import time

# for RPI version 1, use "bus = smbus.SMBus(0)"
bus = smbus.SMBus ( 1 )
 
# address of the slave to address
address = 0x10

class ShadowCommand:

    TC_HEARTBEAT = 1
    TC_GETHELLO = 2

    def __init__ ( self, address ):
        self.address = address

    def sendbuf ( self, reg, values ):
        bus.write_block_data ( self.address, reg, values )
        return None

    def readbuf ( self, reg ):
        return bus.read_byte_data ( self.address, reg )
 
    def sendbyte ( self, value ):
        bus.write_byte ( self.address, value )
        # bus.write_byte_data ( address, 0, value )
        print "# Sent >>", value
        return None
 
    def readbyte ( self ):
        number = bus.read_byte ( self.address )
        # number = bus.read_byte_data ( address, 1 )
        print "# Received <<", number
        return number

shadow = ShadowCommand ( address ) 
first = True

while True:
 
    #shadow.sendbuf ( shadow.TC_HEARTBEAT, [ 0 ] ) # heartbeat command - init
    #shadow.sendbuf ( shadow.TC_GETHELLO, [ 0 ] ) # heartbeat command - init

    # sleep one second
    time.sleep ( 1 )

    retbuf = shadow.readbuf ( shadow.TC_HEARTBEAT )
 
    #retbuf = shadow.readbuf ( shadow.TC_GETHELLO )
    #number = shadow.readbyte()
    #print "Received heartbeat answer: ", number

    print "Received heartbeat answer: ", retbuf
