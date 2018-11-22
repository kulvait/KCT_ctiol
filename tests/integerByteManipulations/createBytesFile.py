#!/usr/bin/python3.5
#First we test to write 16bit number as unsigned little endian
#8bit unsigned int has a range 0...2^8-1=255
#16bit unsigned int has a range 0...2^16-1=65535
#32bit number has a range 0...2^36-1 = 0...4294967295
#We can expect that negative values are represented in "twos complement" system
#8bit signed int has a range -2^7...2^7-1 = -128...127
#16bit signed int has a range -2^15...2^15-1=-32768...32767
#32bit signed int has a range -2^31...2^31-1=-2147483648...2147483647
#We write sequence
import struct

f = open("uint8", "wb")
#uint8_t 0,255
f.write((0).to_bytes(1,byteorder='little',signed=False))
f.write((255).to_bytes(1,byteorder='little',signed=False))
f.close()

f=open("int8", "wb")
#int8_t -128,-1,0,1,127
f.write((-128).to_bytes(1,byteorder='little',signed=True))
f.write((-1).to_bytes(1,byteorder='little',signed=True))
f.write((0).to_bytes(1,byteorder='little',signed=True))
f.write((1).to_bytes(1,byteorder='little',signed=True))
f.write((127).to_bytes(1,byteorder='little',signed=True))
f.close()

f=open("uint16", "wb")
#uint16_t 0,65535
f.write((0).to_bytes(2,byteorder='little',signed=False))
f.write((65535).to_bytes(2,byteorder='little',signed=False))
f.close()

f=open("int16", "wb")
#int16_t -32768,-1,0,1,32767
f.write((-32768).to_bytes(2,byteorder='little',signed=True))
f.write((-1).to_bytes(2,byteorder='little',signed=True))
f.write((0).to_bytes(2,byteorder='little',signed=True))
f.write((1).to_bytes(2,byteorder='little',signed=True))
f.write((32767).to_bytes(2,byteorder='little',signed=True))
f.close()

f=open("uint32", "wb")
#uint32_t 0,4294967295
f.write((0).to_bytes(4,byteorder='little',signed=False))
f.write((4294967295).to_bytes(4,byteorder='little',signed=False))
f.close()

f=open("int32", "wb")
#int32_t -2147483648,-1,0,1,2147483647
f.write((-2147483648).to_bytes(4,byteorder='little',signed=True))
f.write((-1).to_bytes(4,byteorder='little',signed=True))
f.write((0).to_bytes(4,byteorder='little',signed=True))
f.write((1).to_bytes(4,byteorder='little',signed=True))
f.write((2147483647).to_bytes(4,byteorder='little',signed=True))
f.close()

f=open("uint64", "wb")
#uint32_t 0,18446744073709551615
f.write((0).to_bytes(8,byteorder='little',signed=False))
f.write((18446744073709551615).to_bytes(8,byteorder='little',signed=False))
f.close()

#float
f=open("float", "wb")
f.write(struct.pack("<f", 1.1))
f.write(struct.pack("<f", 3.1))
f.write(struct.pack("<f", 340282300000000000000000000000000000000.0))
f.write(struct.pack("<f", -3402.823))
f.write(struct.pack("<f", -3.0e25))

#double
f=open("double", "wb")
f.write(struct.pack("<d", 1.1))
f.write(struct.pack("<d", 3.1))
f.write(struct.pack("<d", 500.0))
f.write(struct.pack("<d", 3.0e253))
f.write(struct.pack("<d", -3.0e253))
