#ifndef USER_HPP
# define USER_HPP
# include "header.hpp"

class User
{
	private:
		std::string _nickName;
		struct pollfd _pollfd;
		int _clientSocket;

	public:
		User();
		User(const User& other);
		User& operator=(const User& other);
		bool operator==(const User& other);
		bool operator!=(const User& other);
		~User();
		std::string	getNickName() const;
		void	setNickName(char *name);
		struct	pollfd getPollFd() const;
		void	setPollFd(int clientSocket);
		int		getFd() const;
		void	setFd(int clientSocket);
};

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

#endif