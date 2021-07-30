#!/usr/bin/env python3
import serial
import sys
import os
import configparser
import time
from decimal import Decimal
import pickle
from enum import Enum

CFG_PATH = "config.cfg.default"
if os.path.exists("config.cfg"):
  CFG_PATH = "config.cfg"
cfg = configparser.ConfigParser()
cfg.read(CFG_PATH)

SERIAL_PORT = cfg.get('Serial', 'port')
SERIAL_BAUDRATE = cfg.getint('Serial', 'baudrate')  
dev = serial.Serial(SERIAL_PORT, SERIAL_BAUDRATE)

CPU_FREQUENCY = cfg.getint('Device', 'cpu_freq')

SAVE_SIGS_FOLDER = "stm32f4_sigs"
SPEED_MEASUREMENT_OUT_FILE = "stm32f4_speed_measurements.txt"
STACK_MEASUREMENT_OUT_FILE = "stm32f4_stack_measurements.txt"


SPX_MLEN = 32

parameter_set_list = ["128f", "128s", "192f", "192s", "256f", "256s", # SHA robust
                      "128f", "128s", "192f", "192s", "256f", "256s", # SHA simple
                      "128f", "128s", "192f", "192s", "256f", "256s", # SHAKE robust
                      "128f", "128s", "192f", "192s", "256f", "256s"] # SHAKE simple

# for iterating over one variant (sha/shake, simple/robust) only  
parameter_set_list_single_variant = ["128f", "128s", "192f", "192s", "256f", "256s"]

# SPX_N, SPX_BYTES, PK_BYTES
paramsets = {
  "128s" : {"SPX_N" : 16, "SPX_BYTES" : 7856, "PK_BYTES" : 32},
  "128f" : {"SPX_N" : 16, "SPX_BYTES" : 17088, "PK_BYTES" : 32},
  "192s" : {"SPX_N" : 24, "SPX_BYTES" : 16224, "PK_BYTES" : 48},
  "192f" : {"SPX_N" : 24, "SPX_BYTES" : 35664, "PK_BYTES" : 48},
  "256s" : {"SPX_N" : 32, "SPX_BYTES" : 29792, "PK_BYTES" : 64},
  "256f" : {"SPX_N" : 32, "SPX_BYTES" : 49856, "PK_BYTES" : 64}
}

stack_results = dict()
speed_results = dict()

# chunk size: SPX_N
def read_stream(dev, data_len, chunk_size):
  data = b''
  i = 0
  while i < data_len:
    dev.write(b'0')
    remaining = data_len - i;
    data += dev.read(min(chunk_size, remaining))
    i += min(chunk_size, remaining)

  return data

# read \n-terminated ASCII strings
def read_line(dev):
  data = b''
  while True:
    c = dev.read(1)
    if c == b'\n':
      return data.decode('ascii')
    data += c


# chunk size: SPX_N
def write_stream(dev, data, data_len, chunk_size):
  i = 0
  while i < data_len:
    dev.read(1)
    dev.write(data[i:i+chunk_size])
    i += chunk_size


def do_speed_measurements():
  for paramset in parameter_set_list:

    #keygen
    dev.write('1'.encode())
    start_keygen = time.time()
    dev.read(1)
    end_keygen = time.time()

    pk = dev.read(paramsets[paramset]["PK_BYTES"])
    sk = dev.read(2*paramsets[paramset]["PK_BYTES"])

    #sign
    dev.write('1'.encode())
    start_sign = time.time()
    dev.read(1)
    end_sign = time.time()

    sig = read_stream(dev, paramsets[paramset]["SPX_BYTES"], paramsets[paramset]["SPX_N"])

    #verify
    dev.write('1'.encode())
    start_verify = time.time()
    dev.read(1)
    end_verify = time.time()

    ret_keygen = int.from_bytes(dev.read(4), byteorder='big', signed=True)
    ret_sign = int.from_bytes(dev.read(4), byteorder='big', signed=True)
    ret_verify = int.from_bytes(dev.read(4), byteorder='big', signed=True)
    print("return codes:", ret_keygen, ret_sign, ret_verify)
  
    paramsetname = read_line(dev)
    print(paramsetname)
    print("keygen time (host measured): ", end_keygen-start_keygen)
    print("sign time (host measured):   ", end_sign-start_sign)
    print("verify time (host measured): ", end_verify-start_verify)
    print("---")

    # store results
    speed_results[paramsetname] = dict()
    speed_results[paramsetname]["pk"] = pk
    speed_results[paramsetname]["sk"] = sk
    speed_results[paramsetname]["sig"] = sig
    speed_results[paramsetname]["keygen_cycles"] = int.from_bytes(dev.read(8), byteorder='big', signed=False)
    speed_results[paramsetname]["sign_cycles"]   = int.from_bytes(dev.read(8), byteorder='big', signed=False)
    speed_results[paramsetname]["verify_cycles"] = int.from_bytes(dev.read(8), byteorder='big', signed=False)

    print("keygen time (from device): ",
          Decimal(speed_results[paramsetname]["keygen_cycles"]) / CPU_FREQUENCY,
          "cycles: ", speed_results[paramsetname]["keygen_cycles"])
    print("sign time (from device):   ",
          Decimal(speed_results[paramsetname]["sign_cycles"]) / CPU_FREQUENCY,
          "cycles: ", speed_results[paramsetname]["sign_cycles"])
    print("verify time (from device): ",
          Decimal(speed_results[paramsetname]["verify_cycles"]) / CPU_FREQUENCY,
          "cycles: ", speed_results[paramsetname]["verify_cycles"])
    print("====\n")

def do_stack_measurements():
  for paramset in parameter_set_list_single_variant:
    
    #keygen
    ret_keygen = int.from_bytes(dev.read(4), byteorder='big', signed=True)
    pk = dev.read(paramsets[paramset]["PK_BYTES"])

    #sign
    sig = read_stream(dev, paramsets[paramset]["SPX_BYTES"], paramsets[paramset]["SPX_N"])
    msg = read_stream(dev, SPX_MLEN, paramsets[paramset]["SPX_N"])
    ret_sign = int.from_bytes(dev.read(4), byteorder='big', signed=True)

    #verify
    write_stream(dev, msg, SPX_MLEN, paramsets[paramset]["SPX_N"])
    write_stream(dev, sig, paramsets[paramset]["SPX_BYTES"], paramsets[paramset]["SPX_N"])
    ret_verify = int.from_bytes(dev.read(4), byteorder='big', signed=True)

    print("return codes:", ret_keygen, ret_sign, ret_verify)
    paramsetname = read_line(dev)

    # store results
    stack_results[paramsetname] = dict()
    stack_results[paramsetname]["keygen_from_main"] = int.from_bytes(dev.read(4), byteorder='big', signed=False)
    stack_results[paramsetname]["keygen_from_start"] = int.from_bytes(dev.read(4), byteorder='big', signed=False)
    stack_results[paramsetname]["sign_from_main"] = int.from_bytes(dev.read(4), byteorder='big', signed=False)
    stack_results[paramsetname]["sign_from_start"] = int.from_bytes(dev.read(4), byteorder='big', signed=False)
    stack_results[paramsetname]["verify_from_main"] = int.from_bytes(dev.read(4), byteorder='big', signed=False)
    stack_results[paramsetname]["verify_from_start"] = int.from_bytes(dev.read(4), byteorder='big', signed=False)

    print(paramsetname)
    print("keygen stack usage (from main): ",
          stack_results[paramsetname]["keygen_from_main"],
          "\nkeygen stack usage (from stack start): ",
          stack_results[paramsetname]["keygen_from_start"])
    print("\nsign stack usage (from main): ",
          stack_results[paramsetname]["sign_from_main"],
          "\nsign stack usage (from stack start): ",
          stack_results[paramsetname]["sign_from_start"])
    print("\nverify stack usage (from main): ",
          stack_results[paramsetname]["verify_from_main"],
          "\nverify stack usage (from stack start): ",
          stack_results[paramsetname]["verify_from_start"])
    print("====\n")


def store_stack_measurements(variant):
  with open('stack_measurements_' + variant + '.pkl', 'wb') as f:
    pickle.dump(stack_results, f, pickle.HIGHEST_PROTOCOL)

def load_stack_measurements(variant):
  global stack_results
  with open('stack_measurements_' + variant + '.pkl', 'rb') as f:
    stack_results = pickle.load(f)

def print_stack_measurements():
  s = ""
  for paramset in stack_results.keys():
    s += paramset + "\n---\n"
    s += "keygen stack usage: "
    s += str(stack_results[paramset]["keygen_from_main"]) + "\n"

    s += "sign stack usage: "
    s += str(stack_results[paramset]["sign_from_main"]) + "\n"

    s += "verify stack usage: "
    s += str(stack_results[paramset]["verify_from_main"])

    s += "\n\n"
  print(s)


def store_speed_measurements():
  with open('speed_measurements.pkl', 'wb') as f:
    pickle.dump(speed_results, f, pickle.HIGHEST_PROTOCOL)

def load_speed_measurements():
  global speed_results
  with open('speed_measurements.pkl', 'rb') as f:
    speed_results = pickle.load(f)

def print_speed_measurements():

  s = ""
  print("keygen (paramset | cycles | seconds)\n---")
  for paramset in speed_results.keys():
    s += paramset + ":\t"

    s += str(speed_results[paramset]["keygen_cycles"]) + "\t"
    s += str(round(Decimal(speed_results[paramset]["keygen_cycles"] / CPU_FREQUENCY), 3))
    s += "\n"
  print(s)

  s = ""
  print("\nsign (paramset | cycles | seconds)\n---")
  for paramset in speed_results.keys():
    s += paramset + ":\t"

    s += str(speed_results[paramset]["sign_cycles"]) + "\t"
    s += str(round(Decimal(speed_results[paramset]["sign_cycles"] / CPU_FREQUENCY), 3))

    s += "\n"
  print(s)

  s = ""
  print("\nverify (paramset | cycles | seconds)\n---")
  for paramset in speed_results.keys():
    s += paramset + ":\t"

    s += str(speed_results[paramset]["verify_cycles"]) + "\t"
    s += str(round(Decimal(speed_results[paramset]["verify_cycles"] / CPU_FREQUENCY), 3)) 

    s += "\n"
  print(s)
                
def write_pk_sk_sig_to_files():
  try:
    if not os.path.exists(SAVE_SIGS_FOLDER):
      os.mkdir(SAVE_SIGS_FOLDER)
  except OSError:
    print("failed to create folder %s" % SAVE_SIGS_FOLDER)

  for paramset in speed_results.keys():
    with open(SAVE_SIGS_FOLDER + "/pk_" + paramset, 'wb') as f:
      f.write(speed_results[paramset]["pk"])
    with open(SAVE_SIGS_FOLDER + "/sk_" + paramset, 'wb') as f:
      f.write(speed_results[paramset]["sk"])
    with open(SAVE_SIGS_FOLDER + "/sig_" + paramset, 'wb') as f:
      f.write(speed_results[paramset]["sig"])
              
if __name__ == "__main__":
  while True:
    val = input("Choose\n(1) Stack measurement\n(2) Speed measurement\n" 
                "(3) Load and print stored stack measurements\n"
                "(4) Load and print stored speed measurements\n")

    if int(val) == 1:
      variant = int(input("Choose\n"
                "(1) SHA2 Robust\n"
                "(2) SHA2 Simple\n" 
                "(3) SHAKE Robust\n"
                "(4) SHAKE Simple\n"))

      chosen_variant = ""
      if variant == 1:
        chosen_variant = "sha2_robust"
      elif variant == 2:
        chosen_variant = "sha2_simple"
      elif variant == 3:
        chosen_variant = "shake_robust"
      elif variant == 4:
        chosen_variant = "shake_simple"
      else:
        print("invalid choice")
        continue

      print("Performing Stack Measurements ...\n===\n")

      dev.write('1'.encode())
      do_stack_measurements()
      store_stack_measurements(chosen_variant)
      
    elif int(val) == 2:
      print("Performing Speed Measurements ...\n===\n")
      dev.write('1'.encode()) 
      do_speed_measurements()
      store_speed_measurements()
      write_pk_sk_sig_to_files()

    elif int(val) == 3:
      variant = int(input("Choose\n"
                "(1) SHA2 Robust\n"
                "(2) SHA2 Simple\n" 
                "(3) SHAKE Robust\n"
                "(4) SHAKE Simple\n"))
      
      chosen_variant = ""
      if variant == 1:
        chosen_variant = "sha2_robust"
      elif variant == 2:
        chosen_variant = "sha2_simple"
      elif variant == 3:
        chosen_variant = "shake_robust"
      elif variant == 4:
        chosen_variant = "shake_simple"
      else:
        print("invalid choice")
        continue
      load_stack_measurements(chosen_variant)
      print_stack_measurements()

    elif int(val) == 4:
      load_speed_measurements()
      print_speed_measurements()

    else:
      continue
