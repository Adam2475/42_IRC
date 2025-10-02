#include "header.hpp"

int main(int ac, char **av, char **envp)
{
    if (ac != 3)
    {
        std::cerr << "usage : ./ircserv <port> <password>" << std::endl;
        return (1);
    }

    // perform parsing on port number
    short int port = atoi(av[1]);
    std::string password = av[2];

    if (server_start(port, password, envp))
        return (1);

    return (0);
}