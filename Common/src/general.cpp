#include "general.h"
#include <iostream>

std::vector <std::string> split(std::string chaine, const char separateur, bool separateurMultiple)
{
	unsigned short i = 0;
	std::vector <std::string> out;
	
	while (chaine[i] != '\0') {
		unsigned short j = i;
		while (chaine[j] != separateur && chaine[j] != '\0') {
			j++;
		}
		if (!separateurMultiple || j != i)
		{
			const unsigned short taille = (j - i)+1;
			char* mot = new char[taille];
			for (unsigned short k = 0; (k + i) < j; k++)
			{
				mot[k] = chaine[i + k];
			}
			mot[taille - 1] = '\0';
			out.push_back(std::string(mot));
			delete mot;
		}
		if (chaine[j] == '\0') break;
		i = j + 1;
	}
	return out;
}



template <class T>
T myParse(std::string _string) {
	T _return = 0;

	if (_string[0] >= '0' && _string[0] <= '9') {
		_return = _string[0] - '0';

		unsigned i = 1;
		while (_string[i] >= '0' && _string[i] <= '9') {
			_return *= 10;
			_return += _string[i] - '0';
			i++;
		}
	}
	else {
		std::cout << "Error : < T myParse(std::string _string) > invalid string _string";
		return -1;
	}

	return _return;
}