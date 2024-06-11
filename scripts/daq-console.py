#!/usr/bin/env python3

import argparse
import struct
import serial
import datetime

# primary buffer to parse
buf = bytearray(0)

def crc16(seed, data):
    crc = seed
    for byte in data:
        crc ^= (byte << 8)
        for _ in range(8):
            if crc & 0x8000:
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1
    return crc & 0xFFFF

def packet(daqAddr, uniq, measType, data):
    now = datetime.datetime.now()
    formatted_time = now.strftime("%Y-%m-%d %H:%M:%S.%f")
    
    # MEAS_TYPE_ADCANALOG
    if measType == 0 and len(data) == 2:
        # NOTE: cannot remember if this should be signed or unsigned
        vals = struct.unpack('!h', data)
        print("%s %04X %1u ANALOG %d" % (formatted_time, uniq, daqAddr, vals[0]))
    
    # MEAS_TYPE_ADCDIGITAL
    elif measType == 1 and len(data) == 1:
        vals = struct.unpack('!B', data)
        print("%s %04X %1u DIGITAL %d" % (formatted_time, uniq, daqAddr, vals[0]))
        
    # MEAS_TYPE_ADCRATE
    elif measType == 2 and len(data) == 2:
        vals = struct.unpack('!H', data)
        print("%s %04X %1u RATE %u" % (formatted_time, uniq, daqAddr, vals[0]))
        
    # MEAS_TYPE_ACCELEROMETER
    elif measType == 3 and len(data) == 12:
        vals = struct.unpack('!iii', data)
        print("%s %04X %1u ACCEL %d %d %d" % (formatted_time, uniq, daqAddr, vals[0], vals[1], vals[2]))
        
    # MEAS_TYPE_STATUS
    elif measType == 4:
        print("%s %04X %1u STATUS" % (formatted_time, uniq, daqAddr))

    
def parse(data):
    global buf
    
    # append data to the buffer
    buf.extend(data);
    
    # only work on buffer with enough data to find a packet
    while len(buf) > 9:
        # find prefix
        try:
            index = buf.index(b'\xAA\x55')
        except ValueError as e:
            # clear the buffer
            buf = bytearray(0)
            return;
    
        # remove everything from the beginning up to and including the prefix
        buf[0:index] = b''
    
        # lets get the header
        header = struct.unpack('!xxBHBB', buf[0:7])
    
        # do we have enough for the header, payload, and checksom
        if len(buf) - 9 < header[3]:
            return
    
        # received checksum
        rsum = struct.unpack('!H', buf[header[3]+7:header[3]+9])
    
        # calculated checksum
        csum = crc16(0xFFFF, buf[2:header[3]+7])
        
        if rsum[0] == csum:
            # process good packet
            packet(header[0], header[1], header[2], buf[7:header[3]+7])
            
            # remove the packet
            buf[0:header[3]+9] = b''
        else:
            # remove the prefix
            buf[0:2] = b''

# primary buffer to parse
buf = bytearray(0)

# Parse command line arguments
parser = argparse.ArgumentParser()
parser.add_argument('port', help='Name of serial port')
parser.add_argument('baud', type=int, help='Baud rate')
args = parser.parse_args()

# Open a connection to the serial port
ser = serial.Serial(port=args.port, baudrate=args.baud)

# Continuously read from serial port
while True:
    if ser.in_waiting > 0:
        # Read the raw serial data as bytes
        data = ser.read(ser.in_waiting)
        
        # parse the data
        parse(data)
