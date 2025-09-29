#include "../include/Server.hpp"

// Orthodoxal Form
Server::Server(){}

Server::Server(const Server& other) : serv_passwd(other.serv_passwd), port(other.port)
{
	for (size_t i = 0; i < other._pollfd.size(); i++)
	{
		_pollfd.push_back(other._pollfd[i]);
	}
}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		serv_passwd = other.serv_passwd;
		for (size_t i = 0; i < other._pollfd.size(); i++)
		{
			_pollfd.push_back(other._pollfd[i]);
		}
		port = other.port;
	}
	return *this;
}

Server::~Server(){}

// Getters

unsigned short Server::getPort() const
{
	return port;
}

std::vector<struct pollfd> Server::getPollFd() const
{
	return _pollfd;
}

const std::string Server::getServPasswd() const
{
	return serv_passwd;
}

// Setters

void Server::setServPasswd(std::string& passwd)
{
	serv_passwd = passwd;
}

void Server::setPort(unsigned short& new_port)
{
	port = new_port;
}

// Necessary functions

/**
 * On going
 * 
 * it will be used to add pollfds to the vector
 * 
 */

void Server::addPollFd(struct pollfd& pollfd)
{
	this->_pollfd.push_back(pollfd);
}

// Check if the argument passed is an 
// unsigned short and in beetween the valid
// limits for a port, and then set the new port value

bool Server::strIsValidPort(const std::string& str)
{
	const char *s = str.c_str();
	if (str.empty() || str.size() > 5)
		return false;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return false;
	}
	unsigned short new_port;
	std::istringstream iss(str);
	if (!(iss >> new_port))
		return false;
	if (new_port > 65535 || new_port == 0)
		return false;
	this->setPort(new_port);
	return true;
}
