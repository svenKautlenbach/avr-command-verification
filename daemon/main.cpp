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
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage:" << std::endl
			<< argv[0] << " [avr serial port]" << std::endl;

		return 1;
	}

	auto avrChannel = utilities::SerialPort(argv[1], 115200, 0, false); 

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
	if (mySessionId != myPid)
	{
		log << "The Session ID " << mySessionId << " does not match the PID - " << std::strerror(errno) << std::endl
			<< "Exiting..." << std::endl;
		return 2;
	}

	if (chdir("/") != 0)
	{
		log << "Could not change the working directory - " << std::strerror(errno) << std::endl
			<< "Exiting..." << std::endl;
	}

	while (1)
	{
		sleep(1);
	}

	return 666;
}
