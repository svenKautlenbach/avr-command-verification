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

		std::string m_url;
	};
}
