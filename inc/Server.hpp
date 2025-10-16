#ifndef SERVER_HPP
#define SERVER_HPP

#include "header.hpp"
#include "User.hpp"
#define PASS "PASS :"
#define NICK "NICK"
#define USER "USER"
#define PRIVMSG "PRIVMSG"
#define JOIN "JOIN"
#define MODE "MODE"
/**
 * es. MODE #channel +i
 *
 *  | Mode | Descrizione                          | Parametro richiesto |
	| ---- | ------------------------------------ | ------------------- |
	| `i`  | Invite-only                          | ❌                  |
	| `t`  | Topic modificabile solo da operatori | ❌                  |
	| `k`  | Imposta password del canale          | ✅ password         |
	| `o`  | Aggiunge/rimuove operatore           | ✅ nickname         |
	| `l`  | Limita numero di utenti              | ✅ numero           |

	| Comando         | Scopo                                              | Esempio                              |
	| --------------- | -------------------------------------------------- | ------------------------------------ |
	| **JOIN**        | entra in un canale                                 | `JOIN #chat`                         |
	| **PART**        | esce da un canale                                  | `PART #chat :Bye!`                   |
	| **PRIVMSG**     | invia messaggio a un canale o utente               | `PRIVMSG #chat :hello world!`        |
	| **NOTICE**      | messaggio non auto-rispondente (es. avvisi server) | `NOTICE #chat :Server restart soon!` | -> da vedere se aggiungere
	| **PING / PONG** | keepalive tra client e server                      | `PING irc.local` / `PONG irc.local`  | -> da vedere se aggiungere
	| **MODE**        | cambia impostazioni di canale o utente             | `MODE #chat +i`                      |
	| **TOPIC**       | imposta o mostra il topic del canale               | `TOPIC #chat :Benvenuti!`            |
	| **KICK**        | rimuove un utente dal canale                       | `KICK #chat bob :Rule violation`     |
	| **INVITE**      | invita un utente a un canale                       | `INVITE bob #chat`                   |
	| **QUIT**        | disconnette l’utente                               | `QUIT :Client exiting`               |

 */ 
// #define USER "USER"
// #define USER "USER"

class Server
{
	private:
		short int   port;
		int			clients;
		int			pClients;
		int			status;
		std::string password;
		int 		serv_fd;
		struct pollfd data;
		std::vector<pollfd> poll_fds;
		char        **envp;
		// std::vector<Channel>	_channels;
		std::vector<User>    	_users;
		// int	checkPassword(int clientSocket);
	public:
		Server();
		Server(short int port, std::string password, char **envp);
		~Server() {
			// close fds here
		};

		///////////////////////////
		// Public methods
		int 	server_start();
		User	userCreation(int clientSocket);
		void 	accept_connections();

		///////////////////////////
		// Getters & Setters
		std::vector<User> getUserVector() const;
		std::string findNickName(int clientFd) const;
		std::string sendReceive(int clientSocket, std::string message);
		void statusPrint(int i, int clientSocket);
};


#endif