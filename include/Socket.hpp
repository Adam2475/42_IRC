#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>

/* struct sockaddr_in {
    sa_family_t    sin_family;   // Address family (AF_INET for IPv4)
    in_port_t      sin_port;     // Port number (16 bits, network byte order)
    struct in_addr sin_addr;     // IPv4 address (32 bits)
    char           sin_zero[8];  // Padding (unused, just to match struct sockaddr size)
}; */

class Socket
{
    private:
        struct sockaddr_in address;
        // refer to the return of the socket function
        int sock;
        // will be the result of the established connection
        int connection;
    public:
        Socket(int domain, int type, int protocol, int port, u_long interface));
        //SOCK_STREAM type = (virtual circuit service)

        virtual int connect_to_network(int sock, struct sockaddr_in address) = 0;

        void test_connection(int);

        // getters and setters
        struct sockaddr_in get_address();
        int get_sock();
        int get_connection();
};

#endif