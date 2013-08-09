#!/usr/bin/python3

# see http://blog.oscarliang.net/raspberry-pi-arduino-connected-i2c/
# trying quick2wire now instead: http://quick2wire.com/articles/how-to-add-quick2wire-as-a-raspbian-software-source/

# i2cdetect -y 1
# (-y 0 for rev1 boards)

import quick2wire.i2c as i2c
import time

# address of the slave to address
address = 0x10

class ShadowCommand:

    TC_HEARTBEAT = 1
    TC_GETHELLO = 2

    def __init__ ( self, address ):
        self.address = address

    def sendbuf ( self, bytes ):
        with i2c.I2CMaster() as bus:
            bus.transaction (
                i2c.writing_bytes ( self.address, bytes )
                )
        return None

    def readbuf ( self ):
        with i2c.I2CMaster() as bus:
            read_results = bus.transaction (
                i2c.reading(address, 8),
                )
        return read_results

shadow = ShadowCommand ( address ) 
first = True

while True:
 
    if 0:
        shadow.sendbuf ( shadow.TC_HEARTBEAT ) # heartbeat command - init
        time.sleep ( 1 )
        retbuf = shadow.readbuf ()
        print ( "Received heartbeat answer: ", retbuf )

    shadow.sendbuf ( shadow.TC_GETHELLO ) # heartbeat command - init
    time.sleep ( 1 )
    retbuf = shadow.readbuf ()
    print ( "Received heartbeat answer: ", retbuf )
