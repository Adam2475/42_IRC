#include "../inc/header.hpp"
#include "../inc/Channel.hpp"

// string::npos è una costante intera, che rappresenta un indice di stringa impossibile (tipo 0xffffffffffffffff o qualcosa del genere)
// e viene usata per segnare la fine di una stringa quando stai iterando, o per indicare un errore quando una sottostringa non viene trovata.

int removeInitialHash(std::string *target)
{
    if (target->find_first_of('#') == std::string::npos)
        return (1);
    else
    {
        // remove first element of array
        target->erase(target->begin());
        return (0);
    }
}


bool channel_error_check(Channel* targetChannel, User& targetUser, std::string& arg2)
{
	if (arg2.c_str() == NULL)
	{
		// ERR_NEEDMOREPARAMS (461)
		std::cout << "need more params" << std::endl;
		return 1;
	}
	std::string mode[10] = {"+i", "-i", "+k", "-k", "+o", "-o", "+l", "-l", "+t", "-t"};
	if (std::find(mode, mode+10, arg2) == mode + 10)
	{
		std::cout << RED << "mode not found" << RESET << std::endl;
		return 1;
	}
	if (!targetChannel)
	{
		// ERR_NOSUCHCHANNEL (403)
		std::cout << "no such channel" << std::endl;
		return 1;

	}
	if (!isInVector(targetUser, targetChannel->getUserVector()))
	{
		// ERR_USERNOTINCHANNEL (441)
		std::cout << "user not in channel" << std::endl;
		return 1;

	}
	if (!isInVector(targetUser, targetChannel->getUserOperatorsVector()))
	{
		std::cout << "nickname dello user: " << targetUser.getNickName() << std::endl;
		// ERR_CHANOPRIVSNEEDED (482)
		std::cout << "not a channel operator" << std::endl;
		return 1;
	}
	return 0;
}