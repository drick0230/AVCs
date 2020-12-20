#include "room.h"

#pragma region ROOM

Room::Room(string _name) : name(_name) {};

void Room::print()
{
	cout << name << endl;
	for (int i = 0; i < userlist.size(); i++)
	{
		cout << userlist[i].pseudo << '\t' << userlist[i].ip << ':' << userlist[i].port << endl;
	}
}

void Room::addUser(user new_user)
{
	for (int i = 0; i < userlist.size(); i++)
	{
		if (new_user.pseudo == userlist[i].pseudo)
		{
			throw string("replicat pseudo");
		}
		if (new_user.ip == userlist[i].ip && new_user.port == userlist[i].port)
		{
			throw string("replicat adresse  " + new_user.ip.toString() + ":" + to_string(new_user.port));
		}
	}
	userlist.push_back(new_user);
}

void Room::addUser(string user_pseudo, sf::IpAddress user_Ip, unsigned short user_port)
{
	user new_user;
	new_user.pseudo = user_pseudo;
	new_user.ip = user_Ip;
	new_user.port = user_port;
	Room::addUser(new_user);
}

bool Room::addUserAdaptative(user new_user)
{
	int adaptNum = -1;
	user userTry = new_user;
	bool finish = false;
	while (!finish)
	{
		finish = true;
		try { addUser(userTry); }
		catch (string msg)
		{
			if (msg == "replicat pseudo")
			{
				finish = false;
				adaptNum++;
				userTry.pseudo = new_user.pseudo + to_string(adaptNum);
			}
			else
			{
				cerr << msg << endl;
				return false;
			}
		}
	}
	return true;
}

bool Room::addUserAdaptative(string user_pseudo, sf::IpAddress user_Ip, unsigned short user_port)
{
	user new_user;
	new_user.pseudo = user_pseudo;
	new_user.ip = user_Ip;
	new_user.port = user_port;
	return Room::addUserAdaptative(new_user);
}

void Room::removeUser(string user_pseudo)
{
	bool find = false;
	for (int i = 0, j=userlist.size() ; i < j; i++)
	{
		if (userlist[i].pseudo == user_pseudo)
		{
			userlist[i] = userlist[j-1];
			userlist.pop_back();
			find = true;
			break;
		}
	}
	if (!find) throw "pseudo inconnu";
}

#pragma endregion

#pragma region Serveur

#pragma endregion

#pragma region client


#pragma endregion