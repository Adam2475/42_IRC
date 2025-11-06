#include "../inc/header.hpp"
#include "../inc/Channel.hpp"

int ft_strlen(const char *str)
{
	if (!str)
		return -1;
	int i = 0;
	while (str[i])
		i++;
	return i;
}

bool	isStrNotAlphaNum(const char *str)
{
	int size = ft_strlen(str);
	if (size < 0)
		return 1;
	for (size_t i = 0; i < ft_strlen(str); i++)
	{
		if (!std::isalnum(static_cast<unsigned char>(str[i])) && str[i] != '\n')
			return 1;
	}
	return 0;
}

bool	isStrNotPrintable(const char *str)
{
	int size = ft_strlen(str);
	if (size < 0)
		return 1;
	for (size_t i = 0; i < ft_strlen(str); i++)
	{
		if (!std::isprint(static_cast<unsigned char>(str[i])) && str[i] != '\n' && str[i] != '\r')
			return 1;
	}
	return 0;
}

int	clearStrCRFL(std::string& received)
{
	if (received.empty())
		return 1;
	for (size_t i = 0; i < received.size(); i++)
	{
		if (received[i] == '\r')
			received.erase(i);
		else if (received[i] == '\n')
			received.erase(i);
	}
	if (received.empty())
		return 1;
	return 0;
}

void pollIn(User& user)
{
	struct pollfd poll = user.getPollFd();
	poll.events = POLLIN;
}

void pollOut(User& user)
{
	struct pollfd poll = user.getPollFd();
	poll.events = POLLOUT;
}

bool	isInVector(User& user, const std::vector<User>& vector)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == user)
			return 1;
	}
	return 0;	
}