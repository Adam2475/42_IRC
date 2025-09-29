#include "../include/Socket.hpp"

Socket::Socket(int domain, int type, int protocol, int port, u_long interface)
{
    //////////////////
    // define address
    //////////////////

    // bind connection to socket
    // it should be set to constants like AF_INET (IPv4) or AF_INET6 (IPv6).
    // htons() means host-to-network short (swaps byte order for 16-bit values).
    address.sin_family = htons(domain);
    // interface where it's running (your ip address)
    // we need to convert the number to be in the byte order for the network
    address.sin_addr.s_addr = htonl(interface);
    address.sin_port = port;

    // create connection
    sock = socket(domain, type, protocol);
    test_connection(sock);
    // need to implement it in parent class before using
    // establish network connection
    connection = connect_to_network(sock, address);
    test_connection(connection);
}

void Socket::test_connection(int item_to_test)
{
    if (item_to_test < 0)
    {
        perror("failed to connect");
        exit(EXIT_FAILURE);
    }
}

        
struct sockaddr_in Socket::get_address()
{
    return address;
}
        
int Socket::get_sock()
{
    return sock;
}
        
int Socket::get_connection()
{
    return connection;
}