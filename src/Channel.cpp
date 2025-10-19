#include "../inc/Channel.hpp"

Channel::Channel()
{
}

//TODO: aggiungere nuovi membri privati nel copy e assignment operator
Channel::Channel(const Channel& other) : _name(other._name), _user_vector(other._user_vector)
{
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other)
	{
		_name = other._name;
		_user_vector = other._user_vector;
	}
	return *this;
}

Channel::~Channel()
{
}

std::vector<User> Channel::getUserVector() const
{
	std::vector<User> new_map(_user_vector);
	return new_map;
}

std::string Channel::getName() const
{
	return _name;
}

Channel::Channel(std::string& name, std::string& passwd, User& creator, std::string& topic
	, unsigned int max_users, bool invite_only, bool topic_restriction) : 
	_name(name), _passwd(passwd), _topic(topic), _max_users(max_users)
	, _invite_only(invite_only), _topic_restriction(topic_restriction)
{
	_user_vector.push_back(creator);
	_operators_vector.push_back(creator);
	std::cout << "Channel " << name << " created successfully!" << std::endl;
}

bool	isInVector(User& user, std::vector<User>& vector)
{
	for (size_t i = 0; i < vector.size(); i++)
	{
		if (vector[i] == user)
			return 1;
		i++;
	}
	return 0;	
}

void	Channel::addUserToChannel(User& user, std::string& passwd)
{
	if (!_passwd.empty() && _passwd.compare(passwd) != 0)
	{
		send(user.getFd(), "Wrong password! Access to channel denied!\n", 43, 0);
		return ;
	}
	if (isInVector(user, _user_vector))
	{
		send(user.getFd(), "You're already part of this channel\n", 37, 0);
		return ;
	}
	else
		_user_vector.push_back(user);
}

void	Channel::inviteUser(User& user, User&user_operator)
{
	if (!isInVector(user_operator, _operators_vector))
	{
		std::cout << "You're not an operator of this Channel!";
		return ;
	}
	else if (!isInVector(user, _user_vector))
	{
		std::cout << "The user isn't part of the Channel!";
		return ;
	}
	else
		_user_vector.push_back(user);
}

void	Channel::addUserToOperatorsVector(User& user, User& user_operator)
{
	if (!isInVector(user_operator, _operators_vector))
	{
		std::cout << "You're not an operator of this Channel!";
		return ;
	}
	else if (!isInVector(user, _user_vector))
	{
		std::cout << "The user isn't part of the Channel!";
		return ;
	}
	else
		_operators_vector.push_back(user);
}

void	Channel::setMaxUsers(unsigned int max_users, User& user_operator)
{
	if (!isInVector(user_operator, _operators_vector))
	{
		std::cout << "You're not an operator of this Channel!";
		return ;
	}
	else
		_max_users = max_users;
}

void	Channel::setName(std::string& name, User& user_operator)
{
	if (!isInVector(user_operator, _operators_vector))
	{
		std::cout << "You're not an operator of this Channel!";
		return ;
	}
	else
		_name = name;
}

void	Channel::kickUser(User &user, User &user_operator)
{
	if (!isInVector(user_operator, _operators_vector))
	{
		write(user_operator.getFd(), "You're not an operator of this Channel!", 40);
		return ;
	}
	else if (!isInVector(user, _user_vector))
	{
		std::cout << "The user isn't part of the Channel!";
		return ;
	}
	else
	{
		std::vector<User>::iterator it;
		int i = 0;
		for (it = _user_vector.begin(); *it != user; ++it)
			_user_vector.erase(it);
		if (isInVector(user, _operators_vector))
		{
			for(it = _operators_vector.begin(); *it != user; ++it)
				_operators_vector.erase(it);
		}
		// TODO: aggiungere messaggio
	}
}
