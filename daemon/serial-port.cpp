#include "serial-port.h"

#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

namespace utilities
{
	SerialPort::SerialPort(const std::string& device, int baud)
	{
		m_fileDescriptor = open(device.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
		if (m_fileDescriptor < 0)
			throw std::runtime_error("Error opening COM port.");

		if (initialize(getTermiosSpeed(baud)) == false)
			throw std::runtime_error("Setting COM port parameters failed.");
	}

	void SerialPort::write(const std::vector<uint8_t>& data)
	{
		auto dataSize = data.size();
		auto writtenData = ::write(m_fileDescriptor, data.data(), dataSize);

		if (writtenData != static_cast<ssize_t>(dataSize))
			throw std::runtime_error("Could not write all the data to serial port.");
	}

	size_t SerialPort::dataAvailable()
	{
		size_t byteCount;
		if (ioctl(m_fileDescriptor, FIONREAD, &byteCount) == -1)
			throw std::runtime_error("Checking available data from serial port resulted in error.");

		return byteCount;
	}

	std::vector<uint8_t> SerialPort::read()
	{
		if (dataAvailable() == 0)
			return {};

		std::vector<uint8_t> data(100);
		auto result = ::read(m_fileDescriptor, data.data(), 100);

		if (result == -1)
			throw std::runtime_error("Reading data from serial port resulted in error.");

		data.resize(result);
		return data;
	}

	SerialPort::~SerialPort()
	{
		close(m_fileDescriptor);
	}

	int SerialPort::getTermiosSpeed(int baud)
	{
		switch (baud)
		{
			case 9600:
				return B9600;
			case 38400:
				return B38400;
			case 57600:
				return B57600;
			case 115200:
				return B115200;
			default:
				throw std::runtime_error("Not supported baud rate");
		};
	}

	bool SerialPort::initialize(int baud)
	{
		struct termios tty;
		memset(&tty, 0, sizeof tty);
		if (tcgetattr(m_fileDescriptor, &tty) != 0)
			return false;

		cfsetospeed(&tty, baud);
		cfsetispeed(&tty, baud);

		tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;	// 8-bit chars.
		tty.c_iflag &= ~IGNBRK;				// Disable break processing.
		tty.c_lflag = 0;				// No signaling chars, no echo,
								// No canonical processing.
		tty.c_oflag = 0;				// No remapping, no delays.
		tty.c_cc[VMIN]  = 0;				// Read doesn't block.
		tty.c_cc[VTIME] = 5;				// 0.5 seconds read timeout.
		tty.c_iflag &= ~(IXON | IXOFF | IXANY);		// Shut off xon/xoff control.
		tty.c_cflag |= (CLOCAL | CREAD);		// Ignore modem controls, enable reading.
		tty.c_cflag &= ~(PARENB | PARODD);		// Shut off parity.
		tty.c_cflag &= ~CSTOPB;				// 1 stop bit.
		tty.c_cflag &= ~CRTSCTS;			// No CTS or RTS.

		if (tcsetattr(m_fileDescriptor, TCSANOW, &tty) != 0)
			return false;

		return true;
	}
}
