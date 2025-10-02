#include "header.hpp"
 #include <netdb.h>

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

static void accept_connections(int sockfd)
{
    // hold the fd of a connected client socket
    int clientSocket;
    // total number of clients
    int clients = 0;
    // number of processed clients
    int pClients = 0;
    // buffer for reading data
    char buffer[1024];
    // store value of recv()
    int status = 0;
    // vector of pollfd structs
    std::vector<pollfd> poll_fds;
    // hold client address used in accept()
    sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);
    // reusable pollfd struct
    pollfd data;

    // add the listening socket to poll_fds
    data.fd = sockfd;
    data.events = POLLIN; // notify incoming data or new connections
    data.revents = 0;
    poll_fds.push_back(data);

    // make the listening socket non blocking
    int bytes_received = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, bytes_received | O_NONBLOCK);

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
            clientSocket = accept(sockfd, (struct sockaddr*)&client_addr, &client_addr_len);
    
            // if there is a new connection accept it
            if (clientSocket > 0)
            {
                std::cout << "new connection accepted" << std::endl;
                data.fd = clientSocket;
                data.events = POLLIN;
                data.revents = 0;
                // add new client to poll_fds
                poll_fds.push_back(data);
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
                    std::cout << "Message from client " << clientSocket 
                              << ": " << buffer << std::endl;
    
                    // i--;
                    // continue;
                }
                else if (status == 0)  // ADD: Handle client disconnection
                {
                    std::cout << "Client " << clientSocket << " disconnected" << std::endl;
                    close(poll_fds[i].fd);
                    poll_fds.erase(poll_fds.begin() + i);
                    clients--;
                    i--; // Adjust index after erasing
                }
            }
        }
    }
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

int server_start(int port, std::string password, char **envp)
{
    int sockfd = -1;
    sockaddr_in srvAddr;

    // filling the sockaddr_in structure;
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_port = htons(port);
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // creating socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "failed to create socket" << std::endl;
        return (1);
    }

    // binding socket to address & port
    if (bind(sockfd, (sockaddr *)&srvAddr, sizeof(srvAddr)) < 0)
    {
        std::cerr << "failed binding the socket" << std::endl;
        close(sockfd);
        return (1);
    }
    std::cout << "socket created and bound successfully" << std::endl;

    // get ip address of local machine
    std::string ip = get_ip_addr(envp);

    // mark the socket to listening mode
    if (listen(sockfd, SOMAXCONN) != 0)
    {
        close(sockfd);
        // free all if allocated
        std::cerr << "not listening" << std::endl;
    }

    std::cout << "Server listening at address: " << ip
        << " on port: " << ntohs(srvAddr.sin_port) << std::endl;

    // accept incoming connections
    accept_connections(sockfd);
    return (0);
}