#ifndef SERVER_HPP
#define SERVER_HPP

#include "header.hpp"
#include "User.hpp"
#define PASS "PASS"
#define NICK "NICK"
#define USER "USER"

class Server
{
	private:
		short int   port;
		int			clients;
		int			pClients;
		int			status;
		std::string password;
		int 		serv_fd;
		struct pollfd data;
		std::vector<pollfd> poll_fds;
		char        **envp;
		// std::vector<Channel>	_channels;
		std::vector<User>    	_users;
		int	checkPassword(int clientSocket);
	public:
		Server();
		Server(short int port, std::string password, char **envp);
		~Server() {
			// close fds here
		};
		int server_start();
		User	userCreation(int clientSocket);
		void accept_connections();
		std::vector<User> getUserVector() const;
		std::string findNickName(int clientFd) const;
		std::string sendReceive(int clientSocket, std::string message);
		void statusPrint(int i, int clientSocket);
};


#endif