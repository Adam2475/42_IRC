#include "header.hpp"

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
		send(recipFd, out.c_str(), out.size(), 0);						
	}
    return (0);
}