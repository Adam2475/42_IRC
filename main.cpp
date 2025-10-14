#include "header.hpp"
#include "Server.hpp"
#include <limits.h>
#include <cstdlib> 

int main(int ac, char **av, char **envp)
{
    if (ac != 3)
    {
        std::cerr << "usage : ./ircserv <port> <password>" << std::endl;
        return (1);
    }

    // TODO: perform parsing on port number
    // Range: The range of port numbers is 0 to 65535. 
    // limit is the range of unsigned short
    char *end;
    long int port_long = std::strtol(av[1], &end, 10);
    if (*end != '\0' || port_long < 0 || port_long >= 65535)
    {
        std::cerr << "invalid port number" << std::endl;
        return (1);
    }

    unsigned short port = static_cast<unsigned short>(port_long);

    std::string password = av[2];

    try
    {
        std::cout << password << " " << av[1] << " " << av[2] << std::endl;
        Server server(port, password, envp);
        server.server_start();
    }
    catch(const std::exception& e)
    {
        std::cerr << "error initializing the server" << e.what() << std::endl;
    }

    return (0);
}