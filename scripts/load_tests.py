#!/usr/bin/python3

import sys
import os
import serial
import json
import subprocess
import threading
import time

def find_relays( target_test, target_pin, connection, assigments ) :

    relays = []
    for rel in assigments:

        target_connections = connection.split( " - " )
        current_relay_pin = assigments[ rel ][ : assigments[ rel ].index( " - " ) ]
        current_relay_pin_assigned_to = assigments[ rel ][ assigments[ rel ].index( " - " ) + len( " - " ) : ]

        if(( target_pin == current_relay_pin ) and ( current_relay_pin_assigned_to in target_connections )) :
            
            relays.append( rel[ rel.index( "#" ) + len( "#" ) : ])
            
    return relays

if( __name__ == "__main__" ):

    print( "Tests configuration file : {0}, Serial port : {1}".format( sys.argv[ 1 ], sys.argv[ 2 ] ));
    tests_file_data = open( sys.argv[ 1 ] )
    json_tests_data = json.load( tests_file_data )
    tests_file_data.close()
    available_commutations = json_tests_data[ "Available commutations" ]
    relay_assigments = json_tests_data[ "Relay assigments" ]
    tests = dict()
    streams = dict()
    
    for test in sorted( json_tests_data.keys() ):

        if( test.startswith( "Test#" )):

            tests[ test ] = json_tests_data[ test ]

    for test in tests:

        fpga_stream = list( "00000000000000000000000000000000000000000000000000000000000000000000000000000000" )
        for pin in tests[ test ]:
            
            current_test = test
            current_pin = pin
            current_pin_assigned_to = tests[ test ][ pin ]
            current_pin_relays = find_relays( current_test, current_pin, current_pin_assigned_to, relay_assigments )
            # if( len( current_pin_relays )) : print( "Pin {0} connection to \"{1}\" assigned on relays \"{2}\" in test \"{3}\" ".format( current_pin, current_pin_assigned_to, current_pin_relays, current_test ))

            for relay in current_pin_relays:

                fpga_stream[ len( fpga_stream ) - int( relay )] = '1'

        fpga_stream = ''.join( fpga_stream )
        streams[ current_test[ current_test.index( "#" ) + len( "#" ) : ]] = fpga_stream
        
    serial_port = serial.Serial( sys.argv[ 2 ])
    print( "Found serial interface on {0}".format( sys.argv[ 2 ]))
    serial_port.baudrate = 115200
    serial_port.write( "#check fpga\r\n".encode() )
    line = serial_port.readline().decode()

    while not line.endswith( "OK!\r\n" ) : line = serial_port.readline().decode()

    for stream in streams:

        print( "Write test # {0} {1}".format( stream, streams[ stream ]))
        serial_port.write( "#write test {0} {1}\r\n".format( stream, streams[ stream ]).encode() )
        line = serial_port.readline().decode()
        while not line.endswith( "OK!\r\n" ) : line = serial_port.readline().decode()
        
    serial_port.close()
    
    sys.exit( 0 )

