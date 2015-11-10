# avr-command-verification

Different modules are separated in the folders:</BR>
**mote** - AVR MCU Arduino sketch</BR>
**daemon** - UNIX daemon that monitors MCU commands and verifies them against the server via REST

#### mote
Consists of single Arduino sketch file that can be simply opened with the Arduino IDE, built and loaded in a same environment.</BR>

#### daemon
Prerequisites:
* CMake environment
* C++11 compatible toolchain

Verified on:
* Mac OS X 10.10.5(Yosemite) built with Apple LLVM 6.0
* Linux Ubuntu 14.04 built with g++ 4.8

To build do the following:</BR>
```
cd daemon/build/
cmake ../
make
```
Usage:</BR>
```
sudo ./avr-command-daemon [serial port] [validation server url/ip]
```
Daemon has log in the `/var/log/avr-command-daemon`
