#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace utilities
{
	class SerialPort
	{
	public:
		SerialPort(const std::string& device, int baud, int parity, bool doesBlock);

		void write(const std::vector<uint8_t>& data);
		size_t dataAvailable();
		std::vector<uint8_t> read();

		~SerialPort();

	private:
		static int getTermiosSpeed(int baud);

		void writeData(const uint8_t* data, size_t length);
		bool initialize(int baud, int parity, bool doesBlock);

		int m_fileDescriptor;
	};
}
