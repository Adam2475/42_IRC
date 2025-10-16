// #pragma once

#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <sstream>
#include <string> // for string type
#include <vector>
#include <deque>
#include <map>

#include <unistd.h> // for atoi
#include <fcntl.h>  // for fcntl(), F_GETFL, F_SETFL, O_NONBLOCK
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include <netinet/in.h> // for sockaddr_in struct
#include <arpa/inet.h> // htons & htonl
#include <sys/socket.h> // for bind() & socket() & listen() & accept()

#include <poll.h>
#include "User.hpp"
#include "Channel.hpp"

// int server_start(int port, std::string password, char **envp);
int     strlen(const char *str);
bool	isStrNotAlphaNum(const char *str);
bool	isStrNotPrintable(const char *str);
int     clearStrCRFL(std::string& received);

//////////////////
// Commands
int		cmdPrivateMsg(std::stringstream &oss, std::vector<User> users, 
            std::vector<pollfd> &poll_fds, const std::string &senderNick);
int		cmdJoin(std::vector<Channel>& _channels, std::stringstream &oss, std::vector<pollfd> &poll_fds);

/////////////////
// Utils
void    setPollOut(std::vector<pollfd> &poll_fds, int targetFd);
void    setPollIn(std::vector<pollfd> &poll_fds, int targetFd);

#endif