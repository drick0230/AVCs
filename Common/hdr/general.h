#pragma once
#include <string>
#include <vector>


std::vector <std::string> split(std::string chaine, const char separateur, bool separateurMultiple = false);

template <class T>
T myParse(std::string);

enum ServerCommand : UINT8
{
	createRoom = 0,
	joinRoom = 1,
	ExitRoom = 2,
	print = 3
};

enum ClientCommand : UINT8
{
	addUser = 0,
	username = 1,
	removeUser = 2
};