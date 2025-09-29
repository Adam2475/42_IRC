#ifndef SERVER_HPP
#define SERVER_HPP
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>
#include "./Client.hpp"
#include <map>

class Server
{
	private:
		std::string serv_passwd;
		std::vector<struct pollfd> _pollfd;
		unsigned short port;

		/**
		 * struct pollfd
		 * {
		 * 		int fd; -> fd del socket da aggiungere al poll()
		 * 		short event; -> definisce l'azione da eseguire
		 * 		short revent; -> dice quello che e' successo
		 * }
		 * 
		 * NB.	event e revent possono fare il confronto bit a bit
		 * 		quindi puoi fare fds[i].revent & POLLIN per capire
		 * 		chi e' in POLLIN
		 * 
		 * int poll(struct pollfd *fds, nfds_t nfds, int timeout);
		 * aspetta che un fd del puntatore a pollfds
		 * sia pronto per operazioni I/O e in tal caso restituisce
		 * il numero di pollfds presenti nella struttura e da
		 * li' devi andarti a cercare quello con POLLIN attivo
		 * nel revents.
		 * nfds_t nfds sta ad indicare il numero di item
		 * componenti il puntatore ed e' un insigned int.
		 * int timeout indica il numero in millisecondi
		 * prima che il tempo di attesa per un fd pronto
		 * venga trovato.
		 * 
		 * 
		 */
		
		protected:
		public:
			Server();
			Server(const Server& other);
			Server& operator=(const Server& other);
			~Server();
			unsigned short getPort() const;
			std::vector<struct pollfd> getPollFd() const;
			const	std::string	getServPasswd() const;
			void	setServPasswd(std::string& passwd);
			void	setPort(unsigned short& ports);
			// macro = POLLIN, POLLOUT, POLLERR, POLLHUP
			void	addPollFd(struct pollfd& pollfd);
			bool	strIsValidPort(const std::string& str);
};

#endif