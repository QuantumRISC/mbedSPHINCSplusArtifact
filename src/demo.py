#!/usr/bin/env python3
import serial
import sys

if len(sys.argv) < 2:
  print("Usage: {0} [DEVICE]".format(sys.argv[0]))
  sys.exit(-1)

dev = serial.Serial(sys.argv[1], 115200)

while True:
  x = dev.readline()
  print(x.decode())

  v = 16975083

  data = v.to_bytes(4, 'big')
  dev.write(data)

  res = int.from_bytes(dev.read(4), 'big')

  print((v*3 - res) == 0)

  x = dev.readline()
  print(x.decode())

