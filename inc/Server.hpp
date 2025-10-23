#ifndef SERVER_HPP
#define SERVER_HPP

#include "header.hpp"
#define PASS "PASS :"
#define NICK "NICK"
#define PART "PART"
#define USER "USER"
#define PRIVMSG "PRIVMSG"
#define INVITE "INVITE"
#define JOIN "JOIN"
#define MODE "MODE"
#define QUIT "QUIT"
#define KICK "KICK"
#include "Channel.hpp"

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
	| **JOIN**        | entra in un canale                                 | `JOIN #chat`                         | OK
	| **PART**        | esce da un canale                                  | `PART #chat :Bye!`                   |	
	| **PRIVMSG**     | invia messaggio a un canale o utente               | `PRIVMSG #chat :hello world!`        |	OK
	| **NOTICE**      | messaggio non auto-rispondente (es. avvisi server) | `NOTICE #chat :Server restart soon!` | -> da vedere se aggiungere
	| **PING / PONG** | keepalive tra client e server                      | `PING irc.local` / `PONG irc.local`  | -> da vedere se aggiungere
	| **MODE**        | cambia impostazioni di canale o utente             | `MODE #chat +i`                      |
	| **TOPIC**       | imposta o mostra il topic del canale               | `TOPIC #chat :Benvenuti!`            |
	| **KICK**        | rimuove un utente dal canale                       | `KICK #chat bob :Rule violation`     |
	| **INVITE**      | invita un utente a un canale                       | `INVITE bob #chat`                   | OK
	| **QUIT**        | disconnette l’utente                               | `QUIT :Client exiting`               |

 */ 
// #define USER "USER"
// #define USER "USER"

class Server
{
	private:
		short int   			port;
		int						clients;
		int						pClients;
		int						status;
		std::string 			password;
		int 					serv_fd;
		struct pollfd 			data;
		std::vector<pollfd> 	poll_fds;
		char        			**envp;
		std::vector<User>		_users;
		std::vector<Channel>	_channels;
		// int	checkPassword(int clientSocket);
	public:
		Server();
		Server(short int port, std::string password, char **envp);
		~Server() {
			// close fds here
		};

		///////////////////////////
		// Public methods
		int 		server_start();
		User		userCreation(int clientSocket);
		void 		accept_connections();
		User		findUserByNick(std::string targetNick);
		User		getUserByFd(int clientSocket);
		Channel*	findChannelByName(std::string channelName);

		///////////////////////////
		// Getters & Setters
		std::vector<User> getUserVector() const;
		std::string findNickName(int clientFd) const;
		std::string sendReceive(int clientSocket, std::string message);
		void statusPrint(int i, int clientSocket);

		// TODO: da vedere se tenerle
		void    setPollIn(int targetFd);
		void    setPollOut(int targetFd);
		void	sendNumeric(int clientSocket, int code, const std::string& arg, const std::string& msg);


		/////////////////////////////
		// Commands
		int		cmdJoin(std::stringstream &oss, User user);
		int		cmdPrivateMsg(std::stringstream &oss, const std::string &senderNick);
		int		cmdPart(std::stringstream &oss, int clientSocket);
		int		cmdInvite(std::stringstream &oss, int clientSocket);
		int		cmdQuit(std::stringstream &oss, int clientSocket);
		int		cmdKick(std::stringstream &oss, int clientSocket);
};


#endif