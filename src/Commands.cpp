#include "../inc/header.hpp"
#include "../inc/Channel.hpp"
#include "../inc/Server.hpp"

void    Server::setPollOut(int targetFd)
{
 	for (size_t i = 1; i < poll_fds.size(); i++)
	{
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
	for (size_t i = 1; i < poll_fds.size(); i++)
	{
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
	size_t sender_idx = getUserIdByName(senderNick);

	if (!(oss >> targetsToken) || targetsToken.empty())
	{
		std::string err = ":server 461 " + senderNick + " PRIVMSG :Not enough parameters\r\n";
		send(_users[sender_idx].getFd(), err.c_str(), err.size(), 0);
		return (1);
	}

	bool target_found;
	std::cout << targetsToken << std::endl;
	std::string target;
	std::string msgBody;
	std::stringstream tss(targetsToken);
	std::getline(oss, msgBody);

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
			//std::cout << "channels_no: " << _channels.size() << " i: " << i << std::endl;
			std::string channelName = target.substr(1);
			while (i < _channels.size())
			{
				std::cout << "channel: " << _channels[i].getName() << " i: " << i << std::endl;
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
			_channels[i].writeToChannel(_users[sender_idx], out);
		else
			send(_users[i].getFd(), out.c_str(), out.size(), 0);
	}
    return (0);
}

// Servers MUST be able to parse arguments in the
// form of a list of target

// Once a user has joined a channel, he receives information about
// all commands his server receives affecting the channel.  This
// includes JOIN, MODE, KICK, PART, QUIT and of course PRIVMSG/NOTICE.

// If a JOIN is successful, the user receives a JOIN message as
// confirmation and is then sent the channel's topic (using RPL_TOPIC) and
// the list of users who are on the channel (using RPL_NAMREPLY), which
// MUST include the user joining.

// JOIN 0 ;	Leave all currently joined
//        	channels.
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
			if (channelIterator->getInviteOnly())
			{
				// ERR_INVITEONLYCHAN (473)
				// esempio mess: ":irc.example.com 473 Mario #secret :Cannot join channel (+i)"
				return 1;
			}
			std::cout << "channel found" << std::endl;
            channelIterator->addUserToChannel(user, pass);
            
            // Standard IRC Replies for JOIN
            std::string join_msg = ":" + user.getNickName() + " JOIN #" + channelName + "\r\n";
            channelIterator->writeToChannel(user, join_msg); // Send to all users in channel

            std::string topic_msg = ":server 332 " + user.getNickName() + " #" + channelName + " :" + channelIterator->getTopic() + "\r\n";
            send(user.getFd(), topic_msg.c_str(), topic_msg.size(), 0);

            std::string users_list;
			
            std::vector<User> channel_users = channelIterator->getUserVector();
            for(size_t i = 0; i < channel_users.size(); ++i) {
                users_list += channel_users[i].getNickName() + " ";
            }
            
            std::string namreply_msg = ":server 353 " + user.getNickName() + " = #" + channelName + " :" + users_list + "\r\n";
            //send(user.getFd(), namreply_msg.c_str(), namreply_msg.size(), 0);
			channelIterator->writeToChannel(user, namreply_msg);

            std::string endofnames_msg = ":server 366 " + user.getNickName() + " #" + channelName + " :End of /NAMES list.\r\n";
            //send(user.getFd(), endofnames_msg.c_str(), endofnames_msg.size(), 0);
			channelIterator->writeToChannel(user, endofnames_msg);
			return (0);
		}
		++channelIterator;
	}
	std::string topic;
	oss >> topic;
	if (channelIterator == _channels.end())
	{
		std::cout << channelName << "channel not found, creating..." << std::endl;
		// name, pass, creator, topic, max users, invite_only, topic restriction
		Channel new_channel(channelName, pass, user, topic, 100, 0, 0);
		_channels.push_back(new_channel);

		// Standard IRC Replies for successful channel creation and join
        std::string join_msg = ":" + user.getNickName() + " JOIN #" + channelName + "\r\n";
        send(user.getFd(), join_msg.c_str(), join_msg.size(), 0);

        std::string topic_msg = ":server 332 " + user.getNickName() + " #" + channelName + " :" + topic + "\r\n";
        send(user.getFd(), topic_msg.c_str(), topic_msg.size(), 0);

        std::string namreply_msg = ":server 353 " + user.getNickName() + " = #" + channelName + " :" + user.getNickName() + "\r\n";
        send(user.getFd(), namreply_msg.c_str(), namreply_msg.size(), 0);

        std::string endofnames_msg = ":server 366 " + user.getNickName() + " #" + channelName + " :End of /NAMES list.\r\n";
        send(user.getFd(), endofnames_msg.c_str(), endofnames_msg.size(), 0);

	}
	std::cout << token << std::endl;
	return (0);
}

int Server::cmdPart(std::stringstream &oss, int clientSocket)
{
    User user = getUserByFd(clientSocket);
    if (user.getNickName().empty()) {
        return 1; // User not found
    }

    std::string channelName;
    oss >> channelName;

    if (channelName.empty())
    {
        // ERR_NEEDMOREPARAMS (461)
        std::string msg = ":server 461 " + user.getNickName() + " PART :Not enough parameters\r\n";
        send(user.getFd(), msg.c_str(), msg.size(), 0);
        return 1;
    }

    if (channelName[0] == '#') {
        channelName = channelName.substr(1);
    }

    Channel* targetChannel = findChannelByName(channelName);

    if (targetChannel == NULL) {
        // ERR_NOSUCHCHANNEL (403)
        std::string msg = ":server 403 " + user.getNickName() + " #" + channelName + " :No such channel\r\n";
        send(user.getFd(), msg.c_str(), msg.size(), 0);
        return 1;
    }

    if (!isInVector(user, targetChannel->getUserVector())) {
        // ERR_NOTONCHANNEL (442)
        std::string msg = ":server 442 " + user.getNickName() + " #" + channelName + " :You're not on that channel\r\n";
        send(user.getFd(), msg.c_str(), msg.size(), 0);
        return 1;
    }

    // Correctly parse the multi-word reason
    std::string reason;
    std::getline(oss, reason);
    if (!reason.empty() && reason[0] == ' ') {
        reason = reason.substr(1);
    }
    if (!reason.empty() && reason[0] == ':'){
        reason = reason.substr(1);
    } else if (reason.empty()) {
        reason = "Leaving"; // Default reason
    }

    // Remove the user from the channel's internal list
	// moved reply message logic to partUser()
    targetChannel->partUser(user, *targetChannel, reason);

    return 0;
}

int		Server::cmdQuit(std::stringstream &oss, int clientSocket)
{
	std::cout << "Detected command QUIT" << std::endl;

	User quittingUser;
    quittingUser = getUserByFd(clientSocket);

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
            //it->writeToChannel(quittingUser, out);
            it->partUser(quittingUser, *it, quit_msg);
        }
    }

	disconnectClient(clientSocket, quit_msg);

	return (0);
}

int		Server::cmdInvite(std::stringstream &oss, int clientSocket)
{
	std::string targetNick;
	std::string channelName;
	oss >> targetNick >> channelName;
	User inviter = getUserByFd(clientSocket);
	User targetUser = findUserByNick(targetNick);
	if (channelName[0] == '#')
	{
        channelName = channelName.substr(1);
    }
	Channel *targetChannel = findChannelByName(channelName);

	if (targetNick.empty() || channelName.empty())
	{
		// ERR_NEEDMOREPARAMS (461)
		std::cout << "wrong number of arguments" << std::endl;
		return (1);
	}
	
	if (targetUser.getNickName().empty())
	{
		std::cout << "user not found" << std::endl;
		//"err 401"
		return (1);
	}

	std::cout << "found user: " << targetUser.getNickName() << std::endl;
	std::cout << channelName << std::endl;

	if (targetChannel == NULL)
	{
		std::cout << "channel not found" << std::endl;
		// "err 403"
		return (1);
	}

	// checks

	// Check if inviter is on the channel
    if (!isInVector(inviter, targetChannel->getUserVector())) {
        std::string err = ":server 442 " + inviter.getNickName() + " #" + channelName + " :You're not on that channel\r\n";
        send(clientSocket, err.c_str(), err.size(), 0);
        return 1;
    }

	// Check if target is already on the channel
    if (isInVector(targetUser, targetChannel->getUserVector())) {
        std::string err = ":server 443 " + inviter.getNickName() + " " + targetNick + " #" + channelName + " :is already on channel\r\n";
        send(clientSocket, err.c_str(), err.size(), 0);
        return 1;
    }

	// send invite
	targetChannel->addToInvited(targetUser);

	// Send RPL_INVITING to inviter
    std::string inviting_msg = ":server 341 " + inviter.getNickName() + " " + targetNick + " #" + channelName + "\r\n";
    send(clientSocket, inviting_msg.c_str(), inviting_msg.size(), 0);

    // Send INVITE to target user
    std::string invite_msg = ":" + inviter.getNickName() + " INVITE " + targetNick + " :#" + channelName + "\r\n";
    send(targetUser.getFd(), invite_msg.c_str(), invite_msg.size(), 0);


	return (0);
}

void Server::sendNumeric(int clientSocket, int code, const std::string& arg, const std::string& msg)
{
    User user = getUserByFd(clientSocket);

    std::ostringstream oss;
    oss << ":" << ":irc.local" << " " 
        << std::setfill('0') << std::setw(3) << code << " "
        << user.getNickName() << " " 
        << arg << " " << msg << "\r\n";

    std::string reply = oss.str();
    send(clientSocket, reply.c_str(), reply.size(), 0);
}

int		Server::cmdTopic(std::stringstream &oss, int clientSocket)
{
	std::string channel_name;
	oss >> channel_name;
	std::cout << "detected command TOPIC" << std::endl;
	std::string arg2;
	oss >> arg2;
	User targetUser = getUserByFd(clientSocket);

	if (channel_name.empty())
	{
		std::cout << "fatal error, no channel topic" << std::endl;
		return (1);
	}
	std::cout << channel_name << std::endl;

	if (removeInitialHash(&channel_name))
	{
		std::cout << "bad formatted arguments, need channel" << std::endl;
	}
	else
	{
		std::cout << "hash removed correctly" << std::endl;
		std::cout << channel_name << std::endl;
	}

	Channel *targetChannel = findChannelByName(channel_name);
	if (!targetChannel)
	{
		std::cout << "fatal error, no channel found" << std::endl;
		exit(1);
	}
	targetChannel->showChannelTopic();

	if (!arg2.empty())
	{
		if (targetChannel->isOperatorUser(targetUser))
		{
			std::cout << "user is operator, TOPIC operation allowed" << std::endl;
			targetChannel->setTopic(arg2);
		}
		else
		{
			std::cout << "User is not operator, operation aborted" << std::endl;
			return (1);
		}
	}
	return (0);
}

int		Server::cmdKick(std::stringstream &oss, int clientSocket)
{
	// KICK <channel> <nick> [<reason>]
	std::string targetNick;
	std::string channelName;
	oss >> channelName >> targetNick;
	User kicker = getUserByFd(clientSocket);
	User targetUser = findUserByNick(targetNick);
	if (channelName.empty() || targetNick.empty())
	{
		// ERR_NEEDMOREPARAMS (461)
		sendNumeric(clientSocket, 461, "ERR_NEEDMOREPARAMS:", "more params needed");
		std::cout << "need more params" << std::endl;
		return 1;
	}
	Channel *targetChannel = findChannelByName(channelName);
	std::string reason;
	std::getline(oss, reason);
	if (channelName[0] != '#' || targetChannel == NULL)
	{
		// ERR_NOSUCHCHANNEL (403)
		std::cout << "no such channel" << std::endl;
		return 1;
    }
	
	channelName = channelName.substr(1);
	std::vector<User> channelUsers = targetChannel->getUserVector();
	std::vector<User> channelOperators = targetChannel->getUserOperatorsVector();
	if (!isInVector(kicker, channelUsers))
	{
		// ERR_NOTONCHANNEL (442)
		std::cout << "kicker not on channel" << std::endl;
		return 1;
	}
	if (!isInVector(kicker, channelOperators))
	{
		// ERR_CHANOPRIVSNEEDED (482)
		std::cout << "kicker not an operator" << std::endl;
		return 1;
	}
	if (!isInVector(targetUser, channelUsers))
	{
		// ERR_USERNOTINCHANNEL (441)
		std::cout << "user not in channel" << std::endl;
	}

	targetChannel->partUser(targetUser, *targetChannel, reason);

	return 0;
}