#include "../inc/header.hpp"

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