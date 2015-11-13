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
	RestValidation::RestValidation(const std::string& url) : m_url(url) {}

	bool RestValidation::verifyAction(int16_t deviceId, const std::string& command)
	{
		(void)deviceId;
		(void)command;

		auto socketDescriptor = connectToServer();

		if (socketDescriptor == -1)
		{
			return false;
		}

		std::ostringstream request;
		request << "POST / HTTP/1.0\r\n"
			<< "Content-type: application/json\r\n"
			<< "Host: " << m_url << "\r\n"
			<< "Accept: application/json\r\n\r\n"
			<< "{\"id\":" << deviceId << ", \"cmd\":\"" << command << "\"}" << std::endl;

		auto requestBody = request.str();
		auto requestLength = requestBody.length();
		auto sendResult = send(socketDescriptor, requestBody.c_str(), requestLength, 0);
		if (sendResult != static_cast<ssize_t>(requestLength))
		{
			close(socketDescriptor);
			return false;
		}

		std::ostringstream response;
		ssize_t receiveResult;
		do
		{
			char responseBuffer[1001];
			receiveResult = recv(socketDescriptor, responseBuffer, 1000, 0);
			if (receiveResult == -1)
			{
				close(socketDescriptor);
				return false;
			}
			
			responseBuffer[receiveResult] = '\0';
			response << responseBuffer;
		}
		while (receiveResult > 0);
		response << std::endl;

		close(socketDescriptor);

		return isActionValid(response.str());
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

		auto socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (socketDescriptor == -1)
			throw std::runtime_error("Could not create a socket to serve a request to " + m_url);

		struct sockaddr_in socketAddress = {};
		socketAddress.sin_family = AF_INET;
		memcpy(&(socketAddress.sin_addr.s_addr), host->h_addr, host->h_length);
		socketAddress.sin_port = htons(3000); // Localhost verification served via this port.

		// Lets set 10 seconds timeout for sending and receiving.
		struct timeval timeout = {10, 0};
		setsockopt(socketDescriptor, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
		setsockopt(socketDescriptor, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

		// If setting up the TCP session fails, invalid socket is returned.
		if (connect(socketDescriptor, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(socketAddress)) == 0)
			return socketDescriptor;

		close(socketDescriptor);
		return -1;
	}

	bool RestValidation::isActionValid(const std::string& response)
	{
		auto statusCodePosition = response.find(" ") + 1;
		auto statusCode = response.substr(statusCodePosition, 3);

		std::cout << "HTTP result code " << statusCode << std::endl;

		// Although in theory the request returns 403 for invalid action, everything but 200 is treated invalid.
		return (statusCode == "200");
	}
}
