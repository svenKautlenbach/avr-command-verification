#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	std::string request;
	std::cin >> request;
	std::cout << "HTTP/1.1 200 OK" << std::endl << std::endl
		<< "Hello from me, this was your request:" << std::endl
		<< request << std::endl;

	return 0;
}
