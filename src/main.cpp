#include "../include/Server.hpp"

int errorBuilder(std::string strerr)
{
	std::cerr << "Error: " << strerr;
	return 1;
}

int main(int ac, char *av[])
{
	// Parser
	if (ac != 3)
		return errorBuilder("Wrong argument count!");

	std::string arg_port(av[1]);
	std::string arg_psswd(av[2]);
	Server irc_serv;
	if (!irc_serv.strIsValidPort(arg_port))
		return errorBuilder("Not a valid port!");
	irc_serv.setServPasswd(arg_psswd);

	
	return 0;
}