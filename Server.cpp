#include "header.hpp"
 #include <netdb.h>
#include "Server.hpp"

/*
struct sockaddr_in {
	short            sin_family;   // e.g. AF_INET
	unsigned short   sin_port;     // e.g. htons(3490)
	struct in_addr   sin_addr;     // see struct in_addr, below
	char             sin_zero[8];  // zero this if you want to
};

struct in_addr {
	unsigned long s_addr;  // load with inet_aton()
};
*/
Server::Server() {}

Server::Server(short int port, std::string password, char **envp) : port(port), password(password), envp(envp)
{
	clients = 0;
	pClients = 0;
	status = 0;
	serv_fd = -1;
}

int strlen(const char *str)
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
	int size = strlen(str);
	if (size < 0)
		return 1;
	for (size_t i = 0; i < strlen(str); i++)
	{
		if (!std::isalnum(static_cast<unsigned char>(str[i])) && str[i] != '\n')
			return 1;
	}
	return 0;
}

int Server::checkPassword(int clientSocket)
{
	char buffer[1024];
	ssize_t n;
	send(clientSocket, "Set password: \n", 16, 0);
	bzero(buffer, sizeof(buffer));
	n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	std::string received;


	if (n <= 0)
		return (1);
	buffer[n] = '\0';
	// Trim trailing CR/LF that clients (nc) send on Enter
	while (n > 0 && (buffer[n - 1] == '\n' || buffer[n - 1] == '\r'))
	{
		buffer[n - 1] = '\0';
		n--;
	}
	received = buffer;
	if (received != password)
	{
		std::cout << received << " wrong password inputted, closing... " << password << " no password"<< std::endl;
		send(clientSocket, "Wrong password: \n", 18, 0);
		return (1);
	}
	password = received;
	return (0);
}

std::string sendReceive(int clientSocket, std::string& message)
{
	std::string msg;
	ssize_t n;
	char buffer[1024];
	
	msg += "Set ";
	msg += message;
	msg += " :";
	send(clientSocket, msg.c_str(), msg.size(), 0);
	bzero(buffer, sizeof(buffer));
	n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (n <= 0)
	{
		static std::string empty;
		msg = "Wrong " + message + "!";
		send(clientSocket, msg.c_str(), msg.size(), 0);
		return empty;
	}
	// Trim trailing CR/LF that clients (nc) send on Enter
	while (n > 0 && (buffer[n - 1] == '\n' || buffer[n - 1] == '\r'))
	{
		buffer[n - 1] = '\0';
		n--;
	}
	buffer[n] = '\0';
	std::string received(buffer);
	if (received.empty() || isStrNotAlphaNum(received.c_str()))
	{
		static std::string empty;
		msg = "Wrong " + message + "!\n";
		send(clientSocket, msg.c_str(), msg.size(), 0);
		return empty;
	}
	return received;
}

User Server::userCreation(int clientSocket)
{
	bool flag = false;
	std::string nick("nickname");
	std::string user("username");
	while (!flag)
	{
		if (checkPassword(clientSocket))
			continue;
		nick = sendReceive(clientSocket, nick);
		if (nick.empty())
		{
			nick = "nickname";
			continue;
		}
		user = sendReceive(clientSocket, user);
		if (user.empty())
		{
			user = "username";
			continue;
		}
		flag = true;
	}
	return User(user, nick, clientSocket);
}

void Server::accept_connections()
{
	// hold the fd of a connected client socket
	int clientSocket;
	// buffer for reading data
	char buffer[1024];
	// hold client address used in accept()
	sockaddr_in client_addr = {};
	socklen_t client_addr_len = sizeof(client_addr);

	// make the listening socket non blocking
	int bytes_received = fcntl(serv_fd, F_GETFL, 0);
	fcntl(serv_fd, F_SETFL, bytes_received | O_NONBLOCK);
	// add the listening socket to poll_fds
	data.fd = serv_fd;
	data.events = POLLIN; // notify incoming data or new connections
	data.revents = 0;
	poll_fds.push_back(data);
	while (true)
	{
		// handle signals
		// sigint
		// sigquit
		// sigstop
		
		// poll for events
		if (poll(poll_fds.data(), poll_fds.size(), -1) == -1)
		{
			// free if necessary
			return ;
		}

		// std::cout << "clients : " << clients << std::endl;

		if (poll_fds[0].revents & POLLIN)
		{
			 std::cout << "New connection incoming..." << std::endl;
			 clientSocket = accept(serv_fd, (struct sockaddr*)&client_addr, &client_addr_len);
	
			// if there is a new connection accept it
			if (clientSocket > 0)
			{
				std::cout << "new connection accepted" << std::endl;   
				User new_user = this->userCreation(clientSocket);
				_users.push_back(new_user);
				std::cout << buffer;
				// add new client to poll_fds
				poll_fds.push_back(new_user.getPollFd());
				// increment total clients
				clients++;
			}
		}

		// skip the listening socket, check all clients
		for (int i = 1; i < (int)poll_fds.size(); i++)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				clientSocket = poll_fds[i].fd;
				bzero(buffer, 1024);
				status = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

	
				if (status > 0)
				{
					buffer[status] = '\0'; // Null terminate the string
					std::cout << "Message from client " << clientSocket << ' ' << this->findNickName(clientSocket)
							  << ": " << buffer << std::endl;
	
					// i--;
					// continue;
				}
				else if (status == 0)  // ADD: Handle client disconnection
				{
					std::cout << "Client " << clientSocket << " disconnected" << std::endl;
					close(poll_fds[i].fd);
					poll_fds.erase(poll_fds.begin() + i);
					_users.erase(_users.begin() + i);
					clients--;
					i--; // Adjust index after erasing
				}
			}
		}
	}
}


std::string Server::findNickName(int clientFd) const
{
	for (int i = 0; i < _users.size(); i++)
	{
		if (_users[i].getFd() == clientFd)
			return std::string(_users[i].getNickName());
	}
	return std::string();		
}

std::vector<User> Server::getUserVector() const
{
	return std::vector<User>(this->_users);
}

static std::string get_ip_addr(char **env)
{
	std::string envStr;

	for (int i = 0; env[i] != NULL; i++)
	{
		envStr = env[i];
		// find return npos if string was not found
		if (envStr.find("SESSION_MANAGER") != std::string::npos)
			break ;
	}
	envStr.erase(0, envStr.find("/") + 1);
	envStr.erase(envStr.find(":"), envStr.length());
	char *IP;
	struct hostent *host_entry;
	host_entry = gethostbyname(envStr.c_str());
	if (host_entry == NULL){
		perror("host entry not found");
		exit(1);
	}
	IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
	return (IP);
}

int Server::server_start()
{
	sockaddr_in srvAddr;

	// filling the sockaddr_in structure;
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(port);
	srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// creating socket
	serv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_fd < 0)
	{
		throw std::runtime_error("failed to create socket");
	}

	// binding socket to address & port
	if (bind(serv_fd, (sockaddr *)&srvAddr, sizeof(srvAddr)) < 0)
	{
		close(serv_fd);
		throw std::runtime_error("failed binding the socket");
	}
	std::cout << "socket created and bound successfully" << std::endl;

	// get ip address of local machine
	std::string ip = get_ip_addr(envp);

	// mark the socket to listening mode
	if (listen(serv_fd, SOMAXCONN) != 0)
	{
		close(serv_fd);
		// free all if allocated
		std::cerr << "not listening" << std::endl;
	}

	std::cout << "Server listening at address: " << ip
		<< " on port: " << ntohs(srvAddr.sin_port) << std::endl;

	// accept incoming connections
	try
	{
		accept_connections();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	return (0);
}