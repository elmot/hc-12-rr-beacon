# This project is a continuation of [RandomRace radio beacon](http://www.randomrace.ru/2015/gsbc/preparation/transmitter/eng/) project

3rd version is based on hc-12 UART radio extender board with custom firmware and no hardware changes.

### Tooling used:

* STM8VL-DISCOVERY board as a flashing probe
* IAR STM-8 Compiler v 8.3
* STM8 Standard Peripheral Library
* STM8CubeMX
* Silicon Labs WDS3 code generator
* CLion IDE

### What you can find here:
* [hc-12 interconnection table](hc-12-inteconnect.md)
* Tools and libraries [documentation](docs)
* STM8 peripheral config [(pdf)](hc-12-rr-beacon.pdf) [(txt)](hc-12-rr-beacon.txt)

### Transmission protocol
#### Sequence
There are five packets sent, each on it's own power level:
 20, 10, 0, -10, -20 dBm

#### Radio packet format
 1. Preamble 4 bytes 0101 packet (produced by transmitter automatically)
 2. Split sequence 0xD3 0x91 0xD3 0x91 
 3. 1 byte station number
 4. 1 byte transmit power
 5. 1 byte inverted transmit power
 6. Sound packet A 13*0xAA
 7. Sound packet B 8*[0xDB 0x6C]
 8. CRC-16, MSB first, except preamble and split (produced by transmitter automatically)

