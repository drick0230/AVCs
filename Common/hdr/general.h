#pragma once
#include <string>
#include <vector>


std::vector <std::string> split(std::string chaine, const char separateur, bool separateurMultiple = false);

template <class T>
T myParse(std::string);