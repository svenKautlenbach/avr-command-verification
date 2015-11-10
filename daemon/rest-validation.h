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
		bool verifyAction(int16_t deviceId, const std::string& command);

		~RestValidation(){}

	private:
		int connectToServer();
		// Parses the server HTTP result code, based on which the decision is done.
		// 200 - valid, 403 - invalid.
		bool isActionValid(const std::string& response);

		std::string m_url;
	};
}
