#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>
#include <map>
// #include <Client.hpp>
// #include "Server.hpp"

int errorBuilder(std::string strerr)
{
	std::cerr << "Error: " << strerr;
	return 1;
}

int main(int ac, char *av[])
{
	if (ac != 3)
		return errorBuilder("Wrong argument count!");
	return 0;
}