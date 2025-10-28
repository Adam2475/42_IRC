#include "../inc/header.hpp"
#include <netdb.h>
#include "../inc/Server.hpp"

Server::Server() {}

Server::Server(short int port, std::string password, char **envp) : port(port), password(password), envp(envp)
{
	clients = 0;
	pClients = 0;
	status = 0;
	serv_fd = -1;
}

std::string Server::sendReceive(int clientSocket, std::string message)
{
	std::string msg;
	int n;
	char buffer[512];
	
	bzero(buffer, sizeof(buffer));
	n = recv(clientSocket, buffer, sizeof(buffer), 0);
	std::string received(buffer);
	size_t pos = 0;
	while (received.find_first_of("\r\n") == std::string::npos)
	{
		bzero(buffer, sizeof(buffer));
		n = recv(clientSocket, buffer, sizeof(buffer), 0);
		received += buffer;
	}
	if(clearStrCRFL(received))
	{
		static std::string empty;
		return empty;
	}
	std::stringstream oss(received);
	std::cout << "received: " << received << std::endl;
	std::string word;
	oss >> word;
	if (word == "PASS")
	{
		word.append(" :");
		if (word == PASS)
		{
			received.erase(0, received.size());
			// :pass
			oss >> received;
			received.erase(0, 1);
			if(clearStrCRFL(received))
			{
				msg = "Wrong password1!\n";
				send(clientSocket, msg.c_str(), msg.size(), 0);		
				static std::string empty;
				return empty;
			}
			if (received == password)
				return received;
			else
			{
				msg = "Wrong password2!\n";
				send(clientSocket, msg.c_str(), msg.size(), 0);
				static std::string empty;
				return empty;
			}
		}
		else
		{
			msg = "Wrong command!\n";
			send(clientSocket, msg.c_str(), msg.size(), 0);
			static std::string empty;
			return empty;
		}
	}
	else if (word == message)
	{
		received.erase(0, received.size());
		oss >> received;
		if(clearStrCRFL(received))
		{
			static std::string empty;
			return empty;
		}
		if (isStrNotPrintable(received.c_str()))
		{
			static std::string empty;
			msg = "Insert a valid " + message + "!";
			send(clientSocket, msg.c_str(), msg.size(), 0);
			return empty;
		}
		return received;
	}
	else
	{
		msg = "Command required " + message + "!\n";
		send(clientSocket, msg.c_str(), msg.size(), 0);
	}
	std::string empty;
	return empty;
}

void Server::disconnectClient(int clientSocket, std::string quitMessage)
{
	User quittingUser = getUserByFd(clientSocket);
    if (quittingUser.getNickName().empty())
	{
        // User may not be fully registered, or already cleaned up.
        // Just ensure the socket is closed and removed from poll list.
        for (size_t i = 0; i < poll_fds.size(); ++i)
		{
            if (poll_fds[i].fd == clientSocket)
			{
                close(poll_fds[i].fd);
                poll_fds.erase(poll_fds.begin() + i);
                break;
            }
        }
        return;
    }

    std::string out = ":" + quittingUser.getNickName() + " QUIT :" + quitMessage + "\r\n";

    // Erase the user from all channels and notify members
    for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
        if (isInVector(quittingUser, it->getUserVector()))
		{
            //it->writeToChannel(quittingUser, out);
            it->partUser(quittingUser, *it, out);
        }
    }

    // Remove poll fd and close socket
    for (size_t i = 0; i < poll_fds.size(); ++i)
	{
        if (poll_fds[i].fd == clientSocket)
		{
            close(poll_fds[i].fd);
            poll_fds.erase(poll_fds.begin() + i);
            break;
        }
    }

    // Remove user from _users vector
    for (size_t i = 0; i < _users.size(); ++i)
	{
        if (_users[i].getFd() == clientSocket)
		{
            _users.erase(_users.begin() + i);
            break;
        }
    }

    std::cout << "Client " << clientSocket << " (" << quittingUser.getNickName() << ") disconnected." << std::endl;
}

User Server::userCreation(int clientSocket, std::string &hostname)
{
	bool flag = false;
	std::string pass;
	std::string nick;
	std::string user;
	while (!flag)
	{
		pass = sendReceive(clientSocket, PASS);
		if (pass.empty())
			continue;
		else if (pass == password)
			std::cout << "correct password\n";
		nick = sendReceive(clientSocket, NICK);
		if (nick.empty())
			continue;
		user = sendReceive(clientSocket, USER);
		if (user.empty())
			continue;
		std::cout << pass << ' ' << nick << ' ' << user << std::endl;
		flag = true;
	}
	return User(user, nick, clientSocket, hostname);
}

void Server::statusPrint(int i, int clientSocket)
{
	std::cout << "Client " << clientSocket << " disconnected" << std::endl;
	close(poll_fds[i].fd);
	poll_fds.erase(poll_fds.begin() + i);
	_users.erase(_users.begin() + i);
	clients--;
}

User Server::getUserByFd(int clientSocket)
{
	User targetUser;
	for (size_t ui = 0; ui < _users.size(); ++ui)
	{
		if (_users[ui].getFd() == clientSocket)
		{
			targetUser = _users[ui]; // copy
		}
	}
	return targetUser;
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

                // Get the client's IP address and use it as the hostname
                std::string hostname = inet_ntoa(client_addr.sin_addr);

				std::cout << "new connection accepted" << std::endl;   
				User new_user = this->userCreation(clientSocket, hostname);
				_users.push_back(new_user);
				// std::cout << buffer << "user created successfully";
				// add new client to poll_fds
				poll_fds.push_back(new_user.getPollFd());
				// increment total clients
				clients++;
			}
		}

		// skip the listening socket, check all clients for messages
		for (int i = 1; i < (int)poll_fds.size(); i++)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				clientSocket = poll_fds[i].fd;
				bzero(buffer, 1024);
				status = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
				// TODO : se il messaggio supera i 512 char te lo tronca in più messaggi
				if (status > 0)
				{
					buffer[status] = '\0'; // Null terminate the string
					std::string buf(buffer);
					while (isStrNotPrintable(buffer) && status > 0)
					{
						send(clientSocket, "Just alpha numeric chars please!\n", 34, 0);
						bzero(buffer, 1024);
						status = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
					}
					if (status <= 0)
					{
						//statusPrint(i, clientSocket);
						disconnectClient(poll_fds[i].fd, "Connection closed");
						i--;
						continue;
					}
					//////////////////////////////////////////////////////////
					// Command Parsing
					std::string received;
					received += buffer;
					
					// ??
					while (received.find_first_of("\r\n") == std::string::npos)
					{
						bzero(buffer, sizeof(buffer));
						status = recv(clientSocket, buffer, sizeof(buffer), 0);
						received += buffer;
					}
					if(clearStrCRFL(received))
					{
						continue;
					}
					std::stringstream oss(received);
					std::cout << "received: " << received << std::endl;
					std::string word;
					oss >> word;
					if (word == PRIVMSG)
					{
						if (cmdPrivateMsg(oss, findNickName(clientSocket)))
							continue;
					}
					else if (word == JOIN)
					{
						User joiningUser;
						joiningUser = getUserByFd(clientSocket);
						if (cmdJoin(oss, joiningUser))
							continue;
					}
					else if (word == PART)
					{
						cmdPart(oss, clientSocket);
						continue;
					}
					else if (word == QUIT)
					{
						cmdQuit(oss, clientSocket);
						status = 0;
						continue;
					}
					else if (word == INVITE)
					{
						cmdInvite(oss, clientSocket);
						continue;
					}
					else if (word == KICK)
					{
						cmdKick(oss, clientSocket);
					}

					// TODO: se non ci sono comandi prima di un mess bisogna dare 421 ERR_UNKNOWNCOMMAND

					///////////////////////////////////////////////////////////

					std::cout << "Message from client " << clientSocket << ' ' << this->findNickName(clientSocket)
							  << ": " << buffer << std::endl;
	
				}
				else if (status == 0)  // ADD: Handle client disconnection
				{
					std::cout << "status == 0" << std::endl;
					//statusPrint(i, clientSocket);
					disconnectClient(poll_fds[i].fd, "Connection closed");
					i--; // Adjust index after erasing
				}
			}
		}
	}
}

User	Server::findUserByNick(std::string targetNick)
{
	User targetUser;
    for (size_t i = 0; i < _users.size(); ++i)
	{
        if (_users[i].getNickName() == targetNick)
		{
            targetUser = _users[i];
            break;
        }
    }
	return targetUser;
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
	if (host_entry == NULL)
	{
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