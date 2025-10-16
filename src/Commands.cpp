#include "../inc/header.hpp"
#include "../inc/Channel.hpp"

void    setPollOut(std::vector<pollfd> &poll_fds, int targetFd)
{
 	for (size_t i = 1; i < poll_fds.size(); i++) {
		if (poll_fds[i].fd == targetFd)
        {
			poll_fds[i].events = poll_fds[i].events + POLLOUT;
			break;
		}
	}   
}

// clear POLLOUT by explicitly restoring the events field (here we restore to POLLIN)
void    setPollIn(std::vector<pollfd> &poll_fds, int targetFd)
{
	for (size_t i = 1; i < poll_fds.size(); i++) {
		if (poll_fds[i].fd == targetFd)
        {
			poll_fds[i].events = POLLIN;
			break;
		}
	}
}

int		cmdPrivateMsg(std::stringstream &oss, std::vector<User> users, std::vector<pollfd> &poll_fds, const std::string &senderNick)
{
    std::cout << "PRIVMSG found" << std::endl;
	std::string targetsToken;

	if (!(oss >> targetsToken) || targetsToken.empty())
	{
		// error: ERR_NEEDMOREPARAMS = 461,
		std::cerr << "not enough parameters" << std::endl;
		return (1);
	}

	bool target_found;
	std::cout << targetsToken << std::endl;
	std::string target;
	std::string msgBody;
	std::stringstream tss(targetsToken);
	oss >> msgBody;
	std::cout << "message body: " << msgBody << std::endl;
	while (std::getline(tss, target, ','))
	{
		if (target.empty())
			return (1);
		// find recipient fd by nickname
		int recipFd = -1;
		for (size_t ui = 0; ui < users.size(); ++ui)
		{
			if (users[ui].getNickName() == target)
			{
				recipFd = users[ui].getFd();
				break;
			}
		}
		if (recipFd == -1)
		{
			//std::string err = "401 " + findNickName(clientSocket) + " " + singleTarget + " :No such nick/channel\r\n";
			//send(clientSocket, err.c_str(), err.size(), 0);
			std::cerr << "user not found" << std::endl;
			return (1);
		}
		// poll_fds[recipFd]
		// build and send the PRIVMSG to the recipient
		std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + msgBody + "\r\n";

        setPollOut(poll_fds, recipFd);
		send(recipFd, out.c_str(), out.size(), 0);
        setPollIn(poll_fds, recipFd);				
	}
    return (0);
}

int		cmdJoin(std::vector<Channel>& _channels, std::stringstream &oss, std::vector<pollfd> &poll_fds)
{
	std::cout << "detected command JOIN" << std::endl;
	std::string token;

	if (!(oss >> token || token.empty()))
	{
		// send "461 JOIN :Not enough parameters\r\n";
		std::cout << "not enough params" << std::endl;
		return (1);
	}

	if (token[0] != '#')
	{
		// "403 "
		std::cout << "no such channel" << std::endl;
		return (1);
	}

	// add parsing and channel creation

	std::cout << token << std::endl;

	//std::cout << oss << std::endl;
	return (0);
}