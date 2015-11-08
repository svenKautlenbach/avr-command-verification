#include "serial-port.h"

#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

namespace utilities
{
	SerialPort::SerialPort(const std::string& device, int baud, int parity, bool doesBlock)
	{
		m_fileDescriptor = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
		if (m_fileDescriptor < 0)
		{
			throw std::runtime_error("Error opening COM port.");
		}

		if (initialize(getTermiosSpeed(baud), parity, doesBlock) == false)
		{
			throw std::runtime_error("Setting COM port parameters failed.");
		}
	}

	void SerialPort::write(const std::vector<uint8_t>& data)
	{
		auto dataSize = data.size();
		auto writtenData = ::write(m_fileDescriptor, data.data(), dataSize);

		if (writtenData != static_cast<ssize_t>(dataSize))
		{
			throw std::runtime_error("Could not write all the data to serial port.");
		}
	}

	size_t SerialPort::dataAvailable()
	{
		return 0;
	}

	std::vector<uint8_t> SerialPort::read()
	{
		return {};
	}

	SerialPort::~SerialPort()
	{
		close(m_fileDescriptor);
	}

	int SerialPort::getTermiosSpeed(int baud)
	{
		(void)baud;

		return 0;
	}

	void SerialPort::writeData(const uint8_t* data, size_t length)
	{
		(void)data;
		(void)length;
	}

	bool SerialPort::initialize(int baud, int parity, bool doesBlock)
	{
		(void)baud;
		(void)parity;
		(void)doesBlock;

		return true;
	}
}
