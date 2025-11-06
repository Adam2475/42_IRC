// #pragma once

#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <sstream>
#include <ostream>
#include <string> // for string type
#include <vector>
#include <deque>
#include <map>
#include <iomanip>


#include <unistd.h> // for atoi
#include <fcntl.h>  // for fcntl(), F_GETFL, F_SETFL, O_NONBLOCK
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <netinet/in.h> // for sockaddr_in struct
#include <arpa/inet.h> // htons & htonl
#include <sys/socket.h> // for bind() & socket() & listen() & accept()

#include <poll.h>
#include "User.hpp"
class Channel;

// int server_start(int port, std::string password, char **envp);
int     strlen(const char *str);
bool	isStrNotAlphaNum(const char *str);
bool	isStrNotPrintable(const char *str);
int     clearStrCRFL(std::string& received);
void    pollIn(User& user);
void    pollOut(User& user);
bool	isInVector(User& user, const std::vector<User>& vector);
int     removeInitialHash(std::string *target);
bool channel_error_check(Channel* targetChannel, User& targetUser, std::string& arg2);

#endif