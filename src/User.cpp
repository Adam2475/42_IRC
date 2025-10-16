#include "../inc/User.hpp"

// Constructors and Distructors

User::User()
{
}

User::User(std::string& userName, std::string& nickName, int clientSocket) : _userName(userName), _nickName(nickName), _clientSocket(clientSocket)
{
	setPollFd(clientSocket);
}

User::User(const User& other) : _userName(other._userName),  _nickName(other._nickName), _clientSocket(other._clientSocket)
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

// Getter and Setters

std::string	User::getUserName() const
{
	return _userName;
}

void User::setUserName(char *name)
{
	std::string username(name);
	_userName = username;
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
	_pollfd.revents = 0;
}

int User::getFd() const
{
	return _clientSocket;
}

void User::setFd(int clientSocket)
{
	_clientSocket = clientSocket;
}

// Operators overload

bool User::operator==(const User& other)
{
	if (_nickName == other.getNickName())
		return 1;
	return 0;
}

bool User::operator!=(const User& other)
{
	if (_nickName != other.getNickName())
		return 1;
	return 0;
}

// member functions
