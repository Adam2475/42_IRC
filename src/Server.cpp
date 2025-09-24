#include "Server.hpp"

// Orthodoxal Form
Server::Server(){}

Server::Server(const Server& other) : serv_name(other.serv_name), pollfds(other.pollfds), port(other.port)
{}

Server& Server::operator=(const Server& other)
{
	if (this != &other)
	{
		serv_name = other.serv_name;
		pollfds = other.pollfds;
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

std::vector<struct pollfds> Server::getPollFds() const
{
	return pollfds;
}

const std::string Server::getName() const
{
	return serv_name;
}

const std::string	Server::getRoot() const
{
	return root;
}

// Setters

void Server::setName(std::string& name)
{
	serv_name = name;
}

void Server::setPort(unsigned short& new_port)
{
	port = new_port;
}

// Necessary functions

void Server::addPollFd(struct pollfds& pollfd)
{
	pollfds.push_back(pollfd);
}