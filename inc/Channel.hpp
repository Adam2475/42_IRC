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
		unsigned int	_max_users;

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
			, unsigned int max_users, bool invite_only, bool topic_restriction);
		~Channel();

		std::vector<User> getUserVector() const;
		std::vector<User> getUserOperatorsVector() const;
		std::string getName() const;
		void	addUserToChannel(User& user, std::string& passwd);
		void	inviteUser(User& user, User&user_operator);
		void	addUserToOperatorsVector(User& user, User& user_operator);
		void	setMaxUsers(unsigned int max_users, User& user_operator);
		void	setName(std::string& name, User& user_operator);
		void	kickUser(User& user, User& user_operator);
		void	partUser(User& user);
		void	writeToChannel(User& user, std::string& buffer);
		// void	inviteUser(User& user, User& user_operator);
		// void	setTopic(std::string& passwd, User& user_operator);
		std::string	getTopic() const;
		// void	removeTopicRestriction(User& user_operator);
		// void	removeTopic(User& user_operator);
		// void	setPasswd(std::string& passwd, User& user_operator);
		// void	removePasswd(std::string& passwd, User& user_operator);
		// void	givePrivilageOpt(std::string& userName, User& user_operator);
		// void	removePrivilageOpt(std::string& userName, User& user_operator);
		void	addToInvited(User& user);
};

#endif