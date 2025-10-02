#ifndef SERVER_HPP
#define SERVER_HPP

#include "header.hpp"

class Server
{
    private:
        short int   port;
        std::string password;
        char        **envp;
    public:
        Server(short int port, std::string password, char **envp);
        ~Server() {
            // close fds here
        };
        int server_start();
};

#endif