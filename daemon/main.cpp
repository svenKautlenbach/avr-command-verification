#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "rest-validation.h"
#include "serial-port.h"

namespace
{
	const std::string logFilePath = "/var/log/avr-command-daemon";
	const std::vector<uint8_t> avrConfirmPacket = {1};
	const std::vector<uint8_t> avrDenyPacket = {0};
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage:" << std::endl
			<< "sudo " << argv[0] << " [avr serial port] [validation server url/ip]" << std::endl;

		return 1;
	}

	auto myPid = fork();
	if (myPid == -1)
	{
		std::cerr << "Fork error - " << std::strerror(errno) << std::endl
			<< "Exiting..." << std::endl;

		return 2;
	}

	// Parent exit.
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

	try
	{
		auto avrChannel = utilities::SerialPort(argv[1], 115200); 
		auto validationEntity = utilities::RestValidation(argv[2]);
		
		while (1)
		{
			sleep(1);

			if (avrChannel.dataAvailable() == 0)
			{
				continue;
			}

			auto avrCommandRequest = avrChannel.read();

			// Does not deal with a case where data might be clogged by multiple requests, so press responsively.
			int16_t deviceId;
			memcpy(&deviceId, avrCommandRequest.data(), 2);
			std::string command(reinterpret_cast<char*>(avrCommandRequest.data() + 2), avrCommandRequest.size() - 2);

			log << "Got from ID " << deviceId << " command " << command << std::endl;

			if (validationEntity.verifyAction(deviceId, command))
			{
				log << "Request was success" << std::endl;
				avrChannel.write(avrConfirmPacket);
			}
			else
			{
				log << "Request denied" << std::endl;
				avrChannel.write(avrDenyPacket);
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		log << "Caught runtime error - " << e.what() << std::endl << "Daemon exits." << std::endl;
		return 4;
	}
	catch (const std::exception& e)
	{
		log << "Caught exception - " << e.what() << std::endl << "Daemon exits." << std::endl;
		return 5;
	}
	catch (...)
	{
		log << "Caught unknown type. Daemon exits" << std::endl;
		return 6;
	}

	return 666;
}
