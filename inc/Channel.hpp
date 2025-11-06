#ifndef CHANNEL_HPP
# define CHANNEL_HPP
# include "header.hpp"

class Channel
{
	private:
		std::vector<User> _user_vector;
		std::vector<User> _operators_vector;
		std::string		_name;
		std::string		_passwd;
		std::string		_topic;
		size_t	_max_users;

		// list of invited users
		std::vector<User> _invited_users;

		// MODE
		// TODO: da settare a false al costruttore
		bool		_invite_only;
		bool		_topic_restriction;
		
	public:
		Channel();
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		Channel(std::string& name, std::string& passwd, User& creator, std::string& topic
			, size_t max_users, bool invite_only, bool topic_restriction);
		~Channel();

		void	addUserToChannel(User& user, std::string& passwd);
		void	inviteUser(User& user, User&user_operator);
		void	addUserToOperatorsVector(User& user, User& user_operator);
		void	removeUserFromVector(User& user, std::vector<User>& vector);
		void	setMaxUsers(size_t max_users, User& user_operator);
		/////////////////////////////////////
		// Channel Menagement
		void	kickUser(User& user, User& user_operator);
		void	partUser(User& user, Channel &channel, std::string msg);
		void	writeToChannel(User& user, std::string& buffer);
		void	showChannelTopic();
		bool	isOperatorUser(User target_user) const;
		// void	inviteUser(User& user, User& user_operator);
		////////////////////////////////////
		// Getters & Setters

		std::vector<User> getUserVector() const;
		std::vector<User> getUserOperatorsVector() const;
		std::string getName() const;
		std::string	getTopic() const;
		bool 	getInviteOnly() const;
		void	setInviteOnly(bool set);
		void	setPassword(std::string& pass);
		void	setMaxUsers(size_t num);
		void	setName(std::string& name, User& user_operator);
		void	setTopic(std::string topic);

		/////////////////////

		void	addToInvited(User& user);

		/////////////////////


		void modeInvite(std::string& arg);
		void modePassword(std::stringstream& oss, std::string& arg);
		void modeMaxUsers(std::stringstream& oss, std::string& arg);
		void modeOperator(std::stringstream& oss, User& user, std::string& arg);
		void modeTopic(std::string& arg);
};

void	Channel::setPassword(std::string &pass)
{
	_passwd = pass;
}

void	Channel::setMaxUsers(size_t num)
{
	_max_users = num;
}

void	Channel::setInviteOnly(bool set)
{
	_invite_only = set;
}

void	Channel::removeUserFromVector(User& user, std::vector<User>& vector)
{
	std::vector<User>::const_iterator it = vector.begin();
	for (it; it != vector.end(); ++it)
	{
		if (*it == user)
			vector.erase(it);
	}
}

void Channel::modeInvite(std::string &arg)
{
	if (arg[0] == '+')
		setInviteOnly(true);
	else if (arg[0] == '-')
		setInviteOnly(false);
}

void Channel::modePassword(std::stringstream& oss, std::string& arg)
{
	std::string str;
	if (arg[0] == '-')
	{
		setPassword(str);
		return ;
	}
	else if (arg[0] == '+')
	{
		oss >> str;
		setPassword(str);
	}
}

void Channel::modeMaxUsers(std::stringstream& oss, std::string& arg)
{
	if (arg[0] == '-')
	{
		setMaxUsers(-1);
		return ;
	}
	if (arg[0] == '+')
	{
		std::string str;
		oss >> str;
		int num =  std::atoi(str.c_str());
		setMaxUsers(num);
	}
}

void Channel::modeOperator(std::stringstream& oss, User& user, std::string& arg)
{
	if (arg[0] == '-')
	{
		if (!isInVector(user, _user_vector))
		{
			// ERR_USERNOTINCHANNEL (441)
			std::cout << "user not in channel" << std::endl;
			return ;
		}
		if (!isInVector(user, _operators_vector))
		{
			// ERR_CHANOPRIVSNEEDED (482)
			std::cout << "cannot apply mode: user is not an operator" << std::endl;
			return ;
		}
		removeUserFromVector(user, _operators_vector);
	}
	if (arg[0] == '+')
	{
		if (!isInVector(user, _user_vector))
		{
			// ERR_USERNOTINCHANNEL (441)
			std::cout << "user not in channel" << std::endl;
			return ;
		}
		if (isInVector(user, _operators_vector))
		{
			// ERR_CHANOPRIVSNEEDED (482)
			std::cout << "cannot apply mode: user is already an operator" << std::endl;
			return ;
		}
		_operators_vector.push_back(user);
	}
}

#endif