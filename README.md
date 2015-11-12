# avr-command-verification

Different modules are separated in the folders:</BR>
**mote** - AVR MCU Arduino sketch</BR>
**daemon** - UNIX daemon that monitors MCU commands and verifies them against the server via REST</BR>
**verification-service** - 2 component REST service that certifies AVR requests based on whitelist

Prerequisites for native PC components:
* CMake environment
* C++11 compatible toolchain

Verified on:
* Mac OS X 10.10.5(Yosemite) built with Apple LLVM 6.0
* Linux Ubuntu 14.04 built with g++ 4.8

#### mote
Consists of single Arduino sketch file that can be simply opened with the [Arduino IDE](https://www.arduino.cc/en/Main/Software), built and loaded in the same environment.</BR>

#### daemon
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

#### verification-service
To build do the following:</BR>
```
cd verification-service/build/
cmake ../
make
```
Usage:</BR>
```
cd verification-service
cp build/avr-whitelist-verify .
sudo bash verification-server.sh &
```

