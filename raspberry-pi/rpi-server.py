#!/usr/bin/python3

# web server
import socketserver
import logging
import os         # makedirs
import json
import string
import datetime
import posixpath
import urllib
import http.server
from http.server import SimpleHTTPRequestHandler

# i2c / twi master
import quick2wire.i2c as i2c
import time
import array
import sys

# setup
#
slave_address = 0x10

server_host = 'fw.skeleton.org'
server_port = 12343

# set up routing table
#

routes = []
routes.append ( ['/dist/', './dist'] )
routes.append ( ['DEFAULT', '/dev/null'] )

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

    def tc_takeover ( self ):
        data = array.array ( 'B', [ shadow.TC_TAKEOVER, 0, 0, 0, 0, 0, 0, 0 ] )
        shadow.sendbuf ( data.tolist() )

    def tc_setmotors ( self, l = 63, r = 63 ):
        data = array.array ( 'B', [ shadow.TC_SETMOTORS, l, r, 0, 0, 0, 0, 0 ] )
        shadow.sendbuf ( data.tolist() )

class RequestHandler ( SimpleHTTPRequestHandler ):

    def translate_path ( self, path ):
        """translate path given routes"""

        original_path = path

        # set default root to cwd
        root = os.getcwd()
        
        # look up routes and set root directory accordingly
        found = False
        for pattern, rootdir in routes:
            if path.startswith(pattern):
                # found match!
                path = path[len(pattern):]  # consume path up to pattern len
                root = rootdir
                found = True
                break
        if found == False:
            logging.warning ( "Routing request %s to default %s" % ( original_path, routes [ 1 ][ 1 ] ) )
            return routes [ 1 ][ 1 ]

        # normalize path and prepend root directory
        path = path.split('?',1)[0]
        path = path.split('#',1)[0]
        path = posixpath.normpath(urllib.unquote(path))
        words = path.split('/')
        words = filter(None, words)
        
        path = root
        for word in words:
            drive, word = os.path.splitdrive(word)
            head, word = os.path.split(word)
            if word in (os.curdir, os.pardir):
                continue
            path = os.path.join(path, word)

        logging.info ( 'Mapped GET path %s to path %s' % ( original_path, path ) )

        return path

    def do_GET ( self ):
        logging.debug ( "GET against path '%s'" % ( self.path ) )
        print ( "do_GET" )

        # parse out ?key=value sillyness
        putargs = dict()
        if '?' in self.path:
            try:
                self.path, crap = self.path.split ( '&', 2 )
            except:
                pass
            self.path, query = self.path.split ( '?', 2 )
            key, value = query.split ( '=', 2 )
            putargs [ key ] = value

        # length handling
        length = -1

        if 'Content-Length' in self.headers:
            try:
                length = int ( self.headers['Content-Length'] ) & 0xFFFF # why do I need to mask this?
                logging.debug ( "Content-Length header implies length is %s" % ( length ) )
            except:
                logging.debug ( "Couldn't determine Content-Length from header" )

        req = dict()
        try:
            paths = self.path.split ( "/", 6 )

            # paths [ 0 ] == nil (leading blank)
            req [ 'basepage' ] = paths [ 1 ]
            req [ 'l' ] = paths [ 2 ]
            req [ 'r' ] = paths [ 3 ]

        except:
            basepage = ''

        logging.debug ( "request looks like POST %s" % ( req ) )

        if req [ 'basepage' ] == 'dpad':
            self.send_response ( 200 ) # okay
            self.send_header ( 'Content-type', 'text/html; charset=utf-8' )
            self.send_header ( 'Content-length', 0 )
            self.end_headers()

            shadow.tc_takeover()
            shadow.tc_setmotors ( int ( req [ 'l' ] ), int ( req [ 'r' ] ) )

        elif req [ 'basepage' ] == 'dist':

            f = open ( "dist/index.html", 'r' )
            text = f.read()
            f.close()

            self.send_response ( 200 ) # okay; the following is the right header sequence
            self.send_header ( 'Content-type', 'text/html; charset=utf-8' )
            self.send_header ( 'Content-length', len ( text ) )
            self.end_headers()

            self.wfile.write ( bytes ( text, 'UTF-8' ) )

        else:
            self.send_response ( 400 ) # shir not be okay

class ThreadingHTTPServer ( socketserver.ThreadingMixIn, http.server.HTTPServer ):
    # use some mixins to make a threading server.. lets see how this works
    pass

shadow = ShadowCommand ( slave_address ) 

server_address = ('', server_port)
RequestHandler.protocol_version = "HTTP/1.0"
httpd = ThreadingHTTPServer ( server_address, RequestHandler )
sa = httpd.socket.getsockname()
logging.info ( "Serving HTTP on " + str(sa[0]) + " port " + str(sa[1]) + "..." )
httpd.serve_forever()
