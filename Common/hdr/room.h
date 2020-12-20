#pragma once
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <thread>			// std::this_thread::sleep_for
#include <mutex>
#include <chrono>			// std::chrono::seconds

using namespace std;

struct user
{
	string			pseudo;
	sf::IpAddress	ip;
	unsigned short	port;
};

class Room
{
protected:
	vector <user> userlist;
public:
	string name;

	Room(string _name);
	void print();
	void addUser(user new_user);
	void addUser(string user_pseudo, sf::IpAddress user_Ip, unsigned short user_port);

	bool addUserAdaptative(user new_user);
	bool addUserAdaptative(string user_pseudo, sf::IpAddress user_Ip, unsigned short user_port);

	void removeUser(string user_pseudo);

};

class Room_server : public Room
{

};

class Room_client : public Room
{

};