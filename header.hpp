#pragma once

#ifndef HEADER_HPP
#define HEADER_HPP

#include <iostream>
#include <string> // for string type
#include <vector>

#include <unistd.h> // for atoi
#include <fcntl.h>  // for fcntl(), F_GETFL, F_SETFL, O_NONBLOCK
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>

#include <netinet/in.h> // for sockaddr_in struct
#include <arpa/inet.h> // htons & htonl
#include <sys/socket.h> // for bind() & socket() & listen() & accept()

#include <poll.h>

int server_start(int port, std::string password, char **envp);

#endif