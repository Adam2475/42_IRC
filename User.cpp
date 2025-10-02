#include "User.hpp"

User::User()
{
}

User::User(const User& other) : _nickName(other._nickName), _clientSocket(other._clientSocket)
{
	_pollfd.fd = other._pollfd.fd;
	_pollfd.events = other._pollfd.events;
}

User& User::operator=(const User& other)
{
	if (this != &other)
	{
		_nickName = other._nickName;
		_clientSocket = other._clientSocket;
		_pollfd.fd = other._pollfd.fd;
		_pollfd.events = other._pollfd.events;
	}
	return *this;
}

User::~User()
{
}

std::string	User::getNickName() const
{
	return _nickName;
}

void User::setNickName(char *name)
{
	std::string nickname(name);
	_nickName = nickname;
}

struct pollfd User::getPollFd() const
{
	return _pollfd;
}

void User::setPollFd(int clientSocket)
{
	_pollfd.fd = clientSocket;
	_pollfd.events = POLLIN;
}

int User::getFd() const
{
	return _clientSocket;
}

void User::setFd(int clientSocket)
{
	_clientSocket = clientSocket;
}
