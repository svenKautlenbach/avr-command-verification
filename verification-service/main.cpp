#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "json11.hpp"

namespace
{
	const std::string verificationSuccessResponse = "200 OK";
	const std::string verificationFailedResponse = "403 Forbidden";

	const std::string whitelistFilePath = "whitelist.conf";

	std::ofstream log("verification.log");
}

static bool isConfiguredInWhitelist(int16_t deviceId, const std::string& command);

int main(int argc, char* argv[])
{
	std::string httpVersion;
	auto sendSuccess = [&]() -> void 
	{
		std::cout << httpVersion << " " << verificationSuccessResponse << "\r\n";
	};

	auto sendFailure = [&]() -> void
	{
		std::cout << httpVersion << " " << verificationFailedResponse << "\r\n";
	};	
	
	try
	{
		// Parse the HTTP version, so that later same version is used in the response.
		// No check is done if it is a POST, which should be used according to the spec.
		// Also the path is discarded.
		std::string requestHeaderMethod;
		std::getline(std::cin, requestHeaderMethod);
		httpVersion = requestHeaderMethod.substr(requestHeaderMethod.length() - 9, 8); // The length of the HTTP/1.x equals 8.	

		std::string request;
		// Parses the request until the last line - that is where the JSON data resides.
		// Also does not check the HTTP headers, since it is not directly
		// relevant for comparing the AVR request against the whitelist.
		while (std::getline(std::cin, request).peek() != EOF);

		std::string jsonParseError;
		auto avrVerificationJson = json11::Json::parse(request, jsonParseError);
		if (!jsonParseError.empty())
		{
			log << "Request parsing error for - " << request << std::endl << jsonParseError << std::endl;
			sendFailure();
			return 0;
		}

		log << "Verifying " << request << std::endl; 
		if (isConfiguredInWhitelist(avrVerificationJson["id"].int_value(), avrVerificationJson["cmd"].string_value()))
		{
			log << "Success" << std::endl;
			sendSuccess();
			return 0;
		}
	}
	catch (const std::exception& e)
	{
		log << "There was exception raised - " << e.what() << std::endl;
	}
	catch (...)
	{
		log << "There was unknown throwed object" << std::endl;	
	}
	
	log << "Failure" << std::endl;
	sendFailure();

	return 0;
}

static std::map<int16_t, std::vector<std::string>> parseWhitelist()
{
	std::map<int16_t, std::vector<std::string>> whitelistEntries;

	std::ifstream inputFile(whitelistFilePath);
	if (!inputFile.is_open() || inputFile.fail())
	{
		log << "Failed to open the whitelist file" << std::endl;
		return whitelistEntries;
	}

	std::string entry;
	// Parse the lines from the file.
	while (std::getline(inputFile, entry).eof() == false)
	{
		// Parse the donfiguration entry fields - device and command. 
		std::stringstream entryFeed(entry);

		// Shady things can happen here if whitelist is not formatted correctly.
		std::string deviceIdSpecifier;
		std::string deviceIdString;
		std::getline(entryFeed, deviceIdSpecifier, ':');
		std::getline(entryFeed, deviceIdString, ' ');
		int16_t deviceIdValue = static_cast<int16_t>(std::stoi(deviceIdString));

		std::string deviceCommandSpecifier;
		std::getline(entryFeed, deviceCommandSpecifier, ':');
		std::string deviceCommand;
		while (std::getline(entryFeed, deviceCommand, ';').eof() == false)
		{
			whitelistEntries[deviceIdValue].push_back(deviceCommand);
		}
	}

	return whitelistEntries;
}

static bool isConfiguredInWhitelist(int16_t deviceId, const std::string& command)
{
	auto whitelistEntries = parseWhitelist();

	if (whitelistEntries.count(deviceId) == 0)
		return false;

	const auto& allowedCommands = whitelistEntries[deviceId];
	if (std::find(allowedCommands.begin(), allowedCommands.end(), command) != allowedCommands.end())
		return true;

	return false;
}
