# avr-command-verification

Different modules are separated in the folders:</BR>
**mote** - AVR MCU Arduino sketch</BR>
**daemon** - UNIX daemon that monitors MCU commands and verifies them against the server via REST</BR>
**verification-service** - 2 component REST service that certifies AVR requests based on the whitelist

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
cmake ..
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
cmake ..
make
```
Usage:</BR>
```
cd verification-service
cp build/whitelist-verify .
sudo bash verification-server.sh &
```
Working principles:</BR>
**verification-server.sh** script contains infinity loop which starts the netcat and feeds its data to the **whitelist-verify** binary which in turn outputs the HTTP response where latter is piped inside the script back to the netcat. Binary does its verification based on the **whitelist.conf** file, where different devices and their actions are specified ([see the file included with the repository](https://github.com/svenKautlenbach/avr-command-verification/blob/master/verification-service/whitelist.conf)). The format of the configuration file has custom human-readable format. One must notice that the server script, verification binary and the whitelist configuration shall reside in the same folder.

Whitelist verification can be separately tested on the command line too. It simply reads the request via standard input(```STDIN/cin```) and outputs the response to standard output (```STDOUT/cout```). For the debugging purposes it creates the logfile called **verification.log** in the same folder where the binary is being run.</BR>
Example:</BR>
```
printf "POST / HTTP/1.0\r\nContent-type: application-json\r\nHost: tere\r\nAccept: application/json\r\n\r\n{\"id\":-22719, \"cmd\":\"BTN1\"}\r\n" | ./whitelist-verify 
```

**Notes**</BR>
The JSON library ```libjson11.a``` is pre-compiled as a static library for x86_64 platforms. If different platform is needed, then check the credits and replace the library included with the repository.

#### Credits
[json11](https://github.com/dropbox/json11) parser is used dealing with the JSON data.
