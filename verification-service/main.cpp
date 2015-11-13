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

	std::ofstream logDebug("verification.log", std::ios::app);
}

static bool isConfiguredInWhitelist(int16_t deviceId, const std::string& command);

int main(int argc, char* argv[])
{
	auto sendSuccess = []() -> void 
	{
		std::cout << "HTTP/1.0 " << verificationSuccessResponse << "\r\n";
	};

	auto sendFailure = []() -> void
	{
		std::cout << "HTTP/1.0 " << verificationFailedResponse << "\r\n";
	};	

	try
	{
		std::string jsonRequest;
		std::getline(std::cin, jsonRequest);

		std::string jsonParseError;
		auto avrVerificationJson = json11::Json::parse(jsonRequest, jsonParseError);
		if (!jsonParseError.empty())
		{
			logDebug << "JSON request parsing error for - " << jsonRequest << std::endl << jsonParseError << std::endl;
			sendFailure();
			return 0;
		}

		logDebug << "Verifying " << jsonRequest << std::endl; 
		if (isConfiguredInWhitelist(avrVerificationJson["id"].int_value(), avrVerificationJson["cmd"].string_value()))
		{
			logDebug << "Success" << std::endl;
			sendSuccess();
			return 0;
		}
	}
	catch (const std::exception& e)
	{
		logDebug << "There was exception raised - " << e.what() << std::endl;
	}
	catch (...)
	{
		logDebug << "There was unknown throwed object" << std::endl;	
	}

	logDebug << "Failure" << std::endl;
	sendFailure();

	return 0;
}

static std::map<int16_t, std::vector<std::string>> parseWhitelist()
{
	std::map<int16_t, std::vector<std::string>> whitelistEntries;

	std::ifstream inputFile(whitelistFilePath);
	if (!inputFile.is_open() || inputFile.fail())
	{
		logDebug << "Failed to open the whitelist file" << std::endl;
		return whitelistEntries;
	}

	std::string entry;
	// Parse the lines from the file.
	// Example of one configuration line:
	// device:<int16> command:<name>;<name2>;
	while (std::getline(inputFile, entry).eof() == false)
	{
		// Parse the configuration entry fields - device and command. 
		std::stringstream entryFeed(entry);

		// Shady things can happen here if the whitelist is not formatted correctly.
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
