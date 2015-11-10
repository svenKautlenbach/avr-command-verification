#include "rest-validation.h"		

#include <cstring>
#include <ctime>
#include <stdexcept>
#include <sstream>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

namespace utilities
{
	RestValidation::RestValidation(const std::string& url) : m_url(url)
	{
	}

	bool RestValidation::verifyAction(int16_t deviceId, const std::string& command)
	{
		static bool result = false;

		(void)deviceId;
		(void)command;

		auto socketDescriptor = connectToServer();

		if (socketDescriptor == -1)
		{
			return false;
		}

		std::ostringstream request;
		request << "GET /category HTTP/1.0\r\n"
			<< "User-Agent: HTMLGET1.0\r\n"
			<< "Host: " << m_url << "\r\n";

		auto requestBody = request.str();
		auto requestLength = requestBody.length();
		std::cout << requestBody;
		auto writeResult = write(socketDescriptor, requestBody.c_str(), requestLength);
		if (writeResult != requestLength)
		{
			std::cout << "Write failed." << std::endl;
			close(socketDescriptor);
			return false;
		}

		char response[1000];
		auto readResult = read(socketDescriptor, response, 1000);
		if (readResult <= 0)
		{
			std::cout << "Read returned " << readResult << " " << strerror(errno) << std::endl;
			close(socketDescriptor);
			return false;
		}

		std::cout << "HTTP GET result:" << std::endl << response;

		result = (result ? false : true);

		return result;
	}

	int RestValidation::connectToServer()
	{
		struct hostent* host = gethostbyname(m_url.c_str());
		if (host == nullptr)
			throw std::runtime_error("Can't get host IP for " + m_url);

		char hostIp[16]; // 4 times XXX plus 3 dots for the standard IPv4 address.
		memset(hostIp, 0, 16);
		if (inet_ntop(AF_INET, static_cast<void*>(host->h_addr_list), hostIp, 15) == nullptr)
		{
			throw std::runtime_error("Cannot stringify the host IP for " + m_url);
		}

		std::cout << m_url << " ip " << hostIp << std::endl;

		auto socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (socketDescriptor == -1)
			throw std::runtime_error("Could not create a socket to serve a request to " + m_url);

		struct sockaddr_in socketAddress = {};
		socketAddress.sin_family = AF_INET;
		memcpy(&(socketAddress.sin_addr.s_addr), host->h_addr, host->h_length);
		socketAddress.sin_port = htons(80);

		// Lets set 10 seconds timeout for sending and receiving.
		struct timeval timeout = {10, 0};
		setsockopt(socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		setsockopt(socketDescriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

		// If setting up TCP session fails, invalid socket is returned.
		if (connect(socketDescriptor, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(socketAddress)) == 0)
			return socketDescriptor;
		
		close(socketDescriptor);
		return -1;
	}
}
