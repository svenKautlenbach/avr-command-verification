#pragma once

#include <string>

namespace utilities
{
	class RestValidation
	{
	public:
		RestValidation(const std::string& url);

		// Sends a request to validation server. When connection or query fails
		// then false is returned. For reattempts, this should be called again. 
		// Each call opens and closes the connection.
		bool verifyAction(int16_t deviceId, const std::string& command);

		~RestValidation(){}

	private:
		// Creates the sockets and sets up the TCP session.
		// On failure returns invalid descriptor(-1).
		int connectToServer();
		// Parses the server HTTP result code, based on which the decision is done.
		// 200 - valid, 403 - invalid.
		bool isActionValid(const std::string& response);

		std::string m_url;
	};
}
