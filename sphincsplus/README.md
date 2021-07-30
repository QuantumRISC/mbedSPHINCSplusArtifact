# Streaming SPHINCS+ Implementation

## Build and Run the code

In the following the separate parts of the code are described and it is described how it can be built and run.
The code uses a hardcoded message for simplicity:

```c
unsigned char m[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                     0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
``` 

Also, for simplicity, no cryptographic PRNG is used but only a linear congruent random number generator. 
For real-world use, this is strongly discouraged and a cryptographic PRNG with a random seed is recommended.
Since the random values are only used as seeds in SPHINCS+, the quality of the randomness does not really matter for the performance evaluation of the code.


### sphincs-sha-shake/

This contains the streaming modifications to the [SPHINCS+ reference code](https://github.com/sphincs/sphincsplus)
A binary for tests and evaluation can be built for unix and stm32f4.

#### Build for Unix:

* `cd sphincs_sha_shake/`
* `make TARGET=unix`
* run with `./build/spx_streaming.elf`

This builds the binary for unix and runs the tests for all parameter sets.
The implementation tests itself against its own non-streaming code (for external verification with the reference implementation, see `sphincs_ref_check/`).
In `out_files/` the produced `sig`, `pk`, `sk` are stored (produced by streaming code)

#### Build for STM32F4:

* `cd sphincs_sha_shake/`
* `make TARGET=stm32f4 spx_streaming.bin`
* flash `build/spx_streaming.bin` to the board, e.g., using `st-flash write build/spx_streaming.bin 0x8000000`

You can look at the `stm32f4.c` file to see what the STM32F4 implementation does, in detail.
A very simple communication protocol is employed: every `SPX_N` bytes, the party that sends data waits to read one byte from the receiving party.
This is done to protect against overflowing the receiver.
The python script in `scripts/` can be used to do the speed and stack measurements.

It is important to note, that currently we build different binaries for:

* speed measurements. One binary is built and all parameter sets are available to be measured.
* stack measurements. One binary is build for each of the four variants: `SHA2_ROBUST`, `SHA2_SIMPLE`, `SHAKE_ROBUST`, `SHAKE_SIMPLE`

This is done mainly because the hash states can differ based on the four variants, i.e., a smaller-sized struct is used for the simple variants.
A more dynamic approach to this can be developed easily (where no separate binaries are required), but currently it's implemented via C preprocessor macros.
The C preprocessor macro appraoch has been chosen in order to facilitate code size measurements (this is currently not properly implemented, though).

Set one of the following makefile-variables to TRUE, the other to FALSE:

```
DO_STACK_MEASUREMENTS
DO_SPEED_MEASUREMENTS
```

Further, the variable `CHOSEN_SINGLE_VARIANT` may refer to any of the four variants which shall be measured.


### scripts/

This directory contains a python script with which the corresponding STM32F4-binary that is built in `sphincs_sha_shake/` can be measured.
Simply run it and choose whether stack or speed measurements are performed.
The results are saved and can be printed at a later time.
Further, the signatures, as well as the public and private keys that the streaming implementation on the board produces are stored in `stm32f4_sigs`


#### config

You can create the file `config.cfg` from the `config.cfg.default` that serves as a template.
Currently, the following variables are contained in the config and can be set to what your setup requires:

```
[Serial]
baudrate = 115200
port = /dev/ttyUSB0

[Device]
cpu_freq = 168000000
```

### sphincs_ref_check/

This contains the reference implementation and can be used to check that the streaming implementation is correct.

#### Build

* First build and run sphincs-sha-shake/ (see above) for unix.
* `cd sphincs_ref_check/`
* `make`
* `./test_all_paramset_against_ref.sh`

This builds all binaries (one for each parameter set) of the reference implementation and then checks the generated streaming implementation signatures and keys.


### sphincs_ref_arm/

This contains the reference implementation and produces binaries for the STM32F4 which output cycle count and stack measurements.

#### Build 

* `cd sphincs_ref_arm/`
* `make`

now all binaries are in `sphincs_ref_arm/spx_bin/` and can be flashed to the board. Results are reported via serial.
          

## License
The code is based on the SPHINCS+ reference code that is available under the CC0 1.0 Universal Public Domain Dedication, 
with the exception of some code that is provided by NIST. 
This can be contained in files that are named rng.c, rng.h and PQCgenKAT_sign.c.
