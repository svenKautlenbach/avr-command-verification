#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace utilities
{
	class SerialPort
	{
	public:
		SerialPort(const std::string& device, int baud);

		void write(const std::vector<uint8_t>& data);
		size_t dataAvailable();
		std::vector<uint8_t> read();

		~SerialPort();

	private:
		static int getTermiosSpeed(int baud);

		bool initialize(int baud);

		int m_fileDescriptor;
	};
}
