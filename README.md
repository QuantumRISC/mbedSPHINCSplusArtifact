# Streaming SPHINCS+ for Embedded Devices at the Example of TPMs

This is the implementation work for the paper Streaming SPHINCS+ for Embedded Devices using the Example of TPMs which can be found at https://eprint.iacr.org/2021/1072.
We present an implementation of the hash-based post-quantum signature scheme SPHINCS+ that enables heavily memory-restricted devices to sign messages by streaming-out a signature during its computation and to verify messages by streaming-in a signature.
We demonstrate our implementation in the context of Trusted Platform Modules (TPMs) by proposing a SPHINCS+ integration and a streaming extension for the TPM specification.

This repository has the following structure:

* [sphincsplus/](sphincsplus) contains the SPHINCS+ reference code with our changes to enable streaming. It can be built stand-alone, i.e., without the TPM firmware. More information is found in the respective README.md of that folder.
* [tpmfirmware/](tpmfirmware) contains the TPM code that is taken from the [Official TPM 2.0 Reference Implementation (by Microsoft)](https://github.com/microsoft/ms-tpm-20-ref). It includes WolfSSL as a git submodule.
* [src/](src) contains our code that integrates the streaming SPHINCS+ code into the TPM.

Clone with ```--recursive``` for external dependencies (wolfSSL).

# Raspberry Pi Software Setup

We tested this with the May 7th 2021 (5.10) Release of Raspberry Pi OS Lite:
Our micro-sd card is available under /dev/sdc but that might differ from system to 
system change /dev/sdX accordingly.


```
wget https://downloads.raspberrypi.org/raspios_lite_armhf/images/raspios_lite_armhf-2021-05-28/2021-05-07-raspios-buster-armhf-lite.zip
unzip 2021-05-07-raspios-buster-armhf-lite.zip   
# writes to 2021-05-07-raspios-buster-armhf-lite.img
# use dd to write to sdcard
sudo dd if=2021-05-07-raspios-buster-armhf-lite.img of=/dev/sdc bs=4K status=progress
```

Either configure the Raspberry Pi to run an ssh server, control it via UART, or connect a monitor and keyboard to it.
[Headless Raspberry Pi How To](https://desertbot.io/blog/headless-raspberry-pi-4-ssh-wifi-setup) might help in this regard.


### Enable SPI after the first boot

The SPI interface has to be enabled in order to reproduce our results.
This can be achieved using the raspi-config tool shipped with raspbian.

```
#Interfaces -> SPI 
sudo raspi-config
```


### Install missing software

We were able to build using the following dependencies:

```
sudo apt install git build-essential autoconf autoconf-archive libssl-dev
```



# Wiring of Raspberry Pi 4 and STM32F4 Discovery 

The SPI controller of the raspberry pi need to be enabled and 
the STM32F4 UART needs to be connected to a serial connector either
on the raspberry pi or a usb device. Wire the raspberry pi and the 
stm32f4 in the following manner.

### Raspberry PI4

```
        3v3 oo 5V             
            oo             
            oo             
            oo             
            oo             
            oo             
            oo             
            oo             
            oo             
      MOSI  oo             
      MISO  oo             
      SCLK  oo CE1            
      GND   oo            
            oo             
            oo             
            oo             
            oo             
            oo             
            oo 
      GND   oo 
```

### STM32F4 DISCO

```
         GND   oo  GND
               oo             
               oo             
               oo             
               oo             
               oo             
UART RX (PA3)  oo  UART TX (PA2)
  SCLK  (PA5)  oo  CE1  (PA4)           
  MOSI  (PA7)  oo  MISO (PA6)             
```


# Reproducing the Measurements

### On the device programming the stm32f4

This requires openocd, arm-none-eabi-gcc, and make.
install them on ubuntu using:
```
apt install make gcc-arm-none-eabi openocd 

```

start openocd 

```
#in src/hal
openocd -f stm32f4.cfg
```

program the stm32f4

```bash
#in src/ 
make TARGET=stm32f4 -j$(nproc)
gdb -ex "set pagination off" -ex "tar extended-remote :3333" -ex "load" -ex "run" build/tpm.elf
```

### On the Raspberry PI

Within this repository cloned on the raspberry pi execute:

```bash
cd tpmfirmware/TPMCmd/
./bootstrap
./configure
make -j$(nproc)
test/command_test -S1
```

The Options for the S-switch are:
```
            0 - sphincs_sha256_128f_robust
            1 - sphincs_sha256_128s_robust
            2 - sphincs_sha256_192f_robust
            3 - sphincs_sha256_192s_robust
            4 - sphincs_sha256_256f_robust
            5 - sphincs_sha256_256s_robust
            6 - sphincs_sha256_128f_simple
            7 - sphincs_sha256_128s_simple
            8 - sphincs_sha256_192f_simple
            9 - sphincs_sha256_192s_simple
           10 - sphincs_sha256_256f_simple
           11 - sphincs_sha256_256s_simple
           12 - sphincs_shake256_128f_robust
           13 - sphincs_shake256_128s_robust
           14 - sphincs_shake256_192f_robust
           15 - sphincs_shake256_192s_robust
           16 - sphincs_shake256_256f_robust
           17 - sphincs_shake256_256s_robust
           18 - sphincs_shake256_128f_simple
           19 - sphincs_shake256_128s_simple
           20 - sphincs_shake256_192f_simple
           21 - sphincs_shake256_192s_simple
           22 - sphincs_shake256_256f_simple
           23 - sphincs_shake256_256s_simple
```
