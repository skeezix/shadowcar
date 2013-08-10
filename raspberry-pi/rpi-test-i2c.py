#!/usr/bin/python3

# see http://blog.oscarliang.net/raspberry-pi-arduino-connected-i2c/
# trying quick2wire now instead: http://quick2wire.com/articles/how-to-add-quick2wire-as-a-raspbian-software-source/

# i2cdetect -y 1
# (-y 0 for rev1 boards)

import quick2wire.i2c as i2c
import time
import array
import sys

# address of the slave to address
slave_address = 0x10

class ShadowCommand:

    TC_HEARTBEAT = 1
    TC_GETHELLO = 2
    TC_SETMOTORS = 3
    TC_TAKEOVER = 4
    TC_RELEASE = 5

    def __init__ ( self, slave_address ):
        self.slave_address = slave_address

    def sendbuf ( self, bytelist ):
        with i2c.I2CMaster() as bus:
            bus.transaction (
                i2c.writing ( self.slave_address, bytes ( bytelist ) )
                )
        return None

    def sendbytes ( self, bytes ):
        with i2c.I2CMaster() as bus:
                bus.transaction (
                    i2c.writing_bytes ( self.slave_address, bytes )
                    )
        return None

    def readbuf ( self, n = 8 ):
        with i2c.I2CMaster() as bus:
            read_results = bus.transaction (
                i2c.reading ( slave_address, n ),
                )
        return read_results

shadow = ShadowCommand ( slave_address ) 

while True:
 
    if 0:
        shadow.sendbuf ( [ shadow.TC_HEARTBEAT, 0, 0 ] ) # heartbeat command - init
        #shadow.sendbytes ( shadow.TC_HEARTBEAT ) # heartbeat command - init
        time.sleep ( 1 )
        retbuf = shadow.readbuf ()
        print ( "Received heartbeat answer: ", retbuf )

    if 0:
        shadow.sendbuf ( [ shadow.TC_GETHELLO, 0, 0 ] ) # heartbeat command - init
        time.sleep ( 1 )
        retbuf = shadow.readbuf ()
        print ( "Received hello answer: ", retbuf )

    if 1:
        data = array.array ( 'B', [ 0, 0, 0, 0, 0, 0, 0, 0 ] )

        print ( "Takeover" );
        data [ 0 ] = shadow.TC_TAKEOVER
        shadow.sendbuf ( data.tolist() )
        time.sleep ( 1 )

        print ( "Motors" );
        data [ 0 ] = shadow.TC_SETMOTORS

        data [ 1 ] = 70
        data [ 2 ] = 60
        shadow.sendbuf ( data.tolist() )
        time.sleep ( 1 )

        data [ 1 ] = 80
        data [ 2 ] = 50
        shadow.sendbuf ( data.tolist() )
        time.sleep ( 1 )

        data [ 1 ] = 90
        data [ 2 ] = 40
        shadow.sendbuf ( data.tolist() )
        time.sleep ( 1 )

        data [ 1 ] = 63
        data [ 2 ] = 63
        shadow.sendbuf ( data.tolist() )
        time.sleep ( 1 )

        sys.exit ( 0 )
