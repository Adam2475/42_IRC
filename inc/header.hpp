// #pragma once

#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <sstream>
#include <ostream>
#include <string> // for string type
#include <vector>
#include <deque>
#include <algorithm>
#include <map>
#include <iomanip>
// --------- COLORI TESTO ---------
#define BLACK   "\033[30m"
#define RED   "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW   "\033[33m"
#define BLUE   "\033[34m"
#define MAGENTA   "\033[35m"
#define CYAN   "\033[36m"
#define WHITE   "\033[37m"

// --------- STILI TESTO ---------
#define BOLD  "\033[1m"
#define UND   "\033[4m"
#define BLINK "\033[5m"

// --------- COLORI SFONDO ---------
#define BBLK  "\033[40m"
#define BRED  "\033[41m"
#define BGRN  "\033[42m"
#define BYEL  "\033[43m"
#define BBLU  "\033[44m"
#define BMAG  "\033[45m"
#define BCYN  "\033[46m"
#define BWHT  "\033[47m"

// --------- RESET ---------
#define RESET "\033[0m"

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
int     ft_strlen(const char *str);
bool	isStrNotAlphaNum(const char *str);
bool	isStrNotPrintable(const char *str);
int     clearStrCRFL(std::string& received);
void    pollIn(User& user);
void    pollOut(User& user);
bool	isInVector(User& user, const std::vector<User>& vector);
int     removeInitialHash(std::string *target);
bool channel_error_check(Channel* targetChannel, User& targetUser, std::string& arg2);

#endif