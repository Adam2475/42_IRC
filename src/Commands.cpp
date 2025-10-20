#include "../inc/header.hpp"
#include "../inc/Channel.hpp"
#include "../inc/Server.hpp"

void    Server::setPollOut(int targetFd)
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
void    Server::setPollIn(int targetFd)
{
	for (size_t i = 1; i < poll_fds.size(); i++) {
		if (poll_fds[i].fd == targetFd)
        {
			poll_fds[i].events = POLLIN;
			break;
		}
	}
}

int		Server::cmdPrivateMsg(std::stringstream &oss, const std::string &senderNick)
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
	// oss >> msgBody;
	// building message string
	std::getline(oss, msgBody);
	std::cout << "message body: " << msgBody << std::endl;
	size_t sender_idx = 0;
    for (size_t j = 0; j < _users.size(); ++j) {
        if (_users[j].getNickName() == senderNick) {
            sender_idx = j;
            break;
        }
    }
	while (std::getline(tss, target, ','))
	{
		bool is_channel = false;
		if (target.empty())
			return (1);
		// find recipient fd by nickname
		// int recipFd = -1;
		size_t i = 0;

		if (target[0] == '#')
		{
			is_channel = true;
			std::cout << "channels_no: " << _channels.size() << "i: " << i << std::endl;
			std::string channelName = target.substr(1);
			while (i < _channels.size())
			{
				std::cout << "channel: " << _channels[i].getName() << "i: " << i << std::endl;
				if (_channels[i].getName() == channelName)
				{
					// recipFd = _users[ui].getFd();
					break;
				}
				i++;
			}
			if (i == _channels.size())
			{
				//std::string err = "401 " + findNickName(clientSocket) + " " + singleTarget + " :No such nick/channel\r\n";
				//send(clientSocket, err.c_str(), err.size(), 0);
				std::cerr << "channel not found" << std::endl;
				return (1);
			}
		}
		else
		{
			while (i < _users.size())
			{
				if (_users[i].getNickName() == target)
				{
					// recipFd = _users[ui].getFd();
					break;
				}
				i++;
			}
			if (i == _users.size())
			{
				//std::string err = "401 " + findNickName(clientSocket) + " " + singleTarget + " :No such nick/channel\r\n";
				//send(clientSocket, err.c_str(), err.size(), 0);
				std::cerr << "user not found" << std::endl;
				return (1);
			}
		}
		// poll_fds[recipFd]
		// build and send the PRIVMSG to the recipient
		std::string out = ":" + senderNick + " PRIVMSG " + target + " :" + msgBody + "\r\n";

		if (is_channel)
		{
			_channels[i].writeToChannel(_users[sender_idx], out);
		}
		else
		{
			pollOut(_users[i]);
			send(_users[i].getFd(), out.c_str(), out.size(), 0);
			pollIn(_users[i]);
		}
	}
    return (0);
}

int		Server::cmdJoin(std::stringstream &oss, User user)
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
	std::string channelName = token.substr(1, token.size() - 1);

	// add parsing and channel creation
	std::string pass;
	oss >> pass;
	std::vector<Channel>::iterator channelIterator = _channels.begin();
	while (channelIterator != _channels.end())
	{
		std::cout << "searching trough channels" << std::endl;
		if (channelName == channelIterator->getName())
		{
			std::cout << "channel found" << std::endl;
			channelIterator->addUserToChannel(user, pass);
			pollOut(user);
			std::string msg = "Welcome to #" + channelIterator->getName() + "!\n";
			send(user.getFd(), msg.c_str(), msg.size(), 0);
			pollIn(user);
			return 0;
		}
		++channelIterator;
	}
	std::string topic;
	oss >> topic;
	if (channelIterator == _channels.end())
	{
		std::cout << channelName << "channel not found, creating..." << std::endl;
		// name, pass, creator, topic, max users, invite_only, topic restriction
		_channels.push_back(Channel(channelName, pass, user, topic, 100, 0, 0));
	}
	std::cout << token << std::endl;

	//std::cout << oss << std::endl;
	return (0);
}

int Server::cmdPart(std::stringstream &oss, int clientSocket)
{
	User user;
	for (size_t i = 0; i < _users.size(); i++)
	{
		if (_users[i].getFd() == clientSocket)
		{
			user = _users[i];
			break;
		}
	}
	std::string token;
	oss >> token;
	if (token.empty())
	{
		pollOut(user);
		std::string msg = "Command usage: PART <channel name> :<bye message>\n";
		send(user.getFd(), msg.c_str(), msg.size(), 0);
		pollIn(user);
		return 1;
	}
	std::string channelName = token.substr(1, token.size() - 1);
	std::vector<Channel>::iterator channelIterator = _channels.begin();
	while (channelIterator != _channels.end())
	{
		// std::cout << "searching trough channels PART COMMAND" << std::endl;
		if (channelName == channelIterator->getName())
		{
			// std::cout << token << " sono qui 1" << std::endl;
			std::vector<User> user_vect = channelIterator->getUserVector();
			if (isInVector(user, user_vect))
			{
				std::string bye_msg;
				oss >> bye_msg;
				// std::cout << bye_msg << " -> bye_msg sono qui 2" << std::endl;
				if (bye_msg[0] != ':')
				{
					pollOut(user);
					std::string msg = "Command usage: PART <channel name> :<bye message>\n";
					send(user.getFd(), msg.c_str(), msg.size(), 0);
					pollIn(user);
					return 1;
				}
				bye_msg += "\r\n";
				channelIterator->writeToChannel(user, bye_msg);
				channelIterator->partUser(user);
			}
			else
			{

				std::cout << "sono qui 3 vect size = " << channelIterator->getUserVector().size() << std::endl;
				pollOut(user);
				std::string msg = "Didn't work!\n";
				send(user.getFd(), msg.c_str(), msg.size(), 0);
				pollIn(user);
				return 1;
			}
			pollOut(user);
			std::string msg = "You aren't part of " + channelIterator->getName() + " anymore. Goodbye!\n";
			send(user.getFd(), msg.c_str(), msg.size(), 0);
			pollIn(user);
			break;
		}
		++channelIterator;
	}
	std::cout <<"sono qui 4" << std::endl;
	return 0;
}

int		Server::cmdQuit(std::stringstream &oss, int clientSocket)
{
	std::cout << "Detected command QUIT" << std::endl;

	User quittingUser;
    size_t user_idx = -1;
    for (size_t i = 0; i < _users.size(); i++)
    {
        if (_users[i].getFd() == clientSocket)
        {
            quittingUser = _users[i];
            user_idx = i;
            break;
        }
    }

	if (user_idx == -1) return 1;

	std::string quit_msg;
    std::getline(oss, quit_msg);
    if (!quit_msg.empty() && quit_msg[0] == ' ')
        quit_msg = quit_msg.substr(1);
    if (quit_msg.empty() || quit_msg[0] != ':')
    {
        quit_msg = ":Client Quit";
    }

	std::string out = ":" + quittingUser.getNickName() + " QUIT " + quit_msg + "\r\n";

    // Erasing the user from all channels
    for (std::vector<Channel>::reverse_iterator it = _channels.rbegin(); it != _channels.rend(); ++it)
    {
        if (isInVector(quittingUser, it->getUserVector()))
        {
            it->writeToChannel(quittingUser, out);
            it->partUser(quittingUser);
        }
    }

	return (0);
}