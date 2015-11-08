#include <fstream>
#include <iostream>
#include <string>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "serial-port.h"

namespace
{
	const std::string logFilePath = "/var/log/avr-command-daemon";
	const std::vector<uint8_t> avrConfirmPacket = {'Y'};
	const std::vector<uint8_t> avrDenyPacket = {'0'};
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage:" << std::endl
			<< "sudo " << argv[0] << " [avr serial port]" << std::endl;

		return 1;
	}

	auto avrChannel = utilities::SerialPort(argv[1], 115200); 

	auto myPid = fork();
	if (myPid == -1)
	{
		std::cerr << "Fork error - " << std::strerror(errno) << std::endl
			<< "Exiting..." << std::endl;

		return 2;
	}

	if (myPid != 0)
	{
		return 0;
	}

	std::ofstream log(logFilePath);
	if (!log.is_open() || !log.good())
	{
		return 1;
	}

	log << "Daemon started." << std::endl <<  "PID:" << myPid << std::endl;

	umask(0);
	auto mySessionId = setsid();
	if (mySessionId == -1)
	{
		log << "Setting session ID resulted in error - " << std::strerror(errno) << std::endl
			<< "Exiting..." << std::endl;
		return 2;
	}

	if (chdir("/") != 0)
	{
		log << "Could not change the working directory - " << std::strerror(errno) << std::endl
			<< "Exiting..." << std::endl;
		return 3;
	}

	while (1)
	{
		static bool confirmed = false;
		sleep(1);
		
		if (avrChannel.dataAvailable() == 0)
			continue;

		auto avrCommandRequest = avrChannel.read();
		log << "Got from AVR: " << std::string(reinterpret_cast<char*>(avrCommandRequest.data()), avrCommandRequest.size()) << std::endl;
		if (confirmed)
		{
			avrChannel.write(avrConfirmPacket);
		}
		else
		{
			avrChannel.write(avrDenyPacket);
		}

		confirmed = (confirmed ? false : true);
	}

	return 666;
}
