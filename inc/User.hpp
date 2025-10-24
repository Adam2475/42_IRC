#pragma once
#ifndef USER_HPP
# define USER_HPP
// # include "header.hpp"
# include <iostream>
# include <poll.h>

class User
{
	private:
		std::string _userName;
		std::string _nickName;
		std::string _hostname;
		struct pollfd _pollfd;
		int _clientSocket;
		
	public:
		User();
		User(std::string& userName, std::string& nickName, int clientSocket, const std::string& hostname);
		User(const User& other);
		User& operator=(const User& other);
		bool operator==(const User& other) const;
		bool operator!=(const User& other);
		~User();

		/////////////////////////////
		// Getters & Setters
		std::string		getUserName() const;
		void			setUserName(char *name);
		std::string		getNickName() const;
		void			setNickName(char *name);
		struct	pollfd 	getPollFd() const;
		void			setPollFd(int clientSocket);
		int				getFd() const;
		void			setFd(int clientSocket);
		std::string		getHostName() const;
};

#endif