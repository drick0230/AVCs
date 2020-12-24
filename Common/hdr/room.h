#pragma once
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>			// std::this_thread::sleep_for
#include <mutex>
#include <chrono>			// std::chrono::seconds
#include "general.h"

using namespace std;

struct user
{
	string			pseudo = "default";
	sf::IpAddress	ip = sf::IpAddress::Any;
	unsigned short	port = 0;
};

struct userSocket
{
	sf::TcpSocket* socketPtr = NULL;
	string pseudo;
};

class Room
{
protected:
	vector <user> listeUser;
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

class Room_server : protected Room
{
protected:
	vector<userSocket> listeSocket;
public:
	Room_server(string _name);

	void addUser(user new_user, sf::TcpSocket* socketPtr);
	void removeUser(string user_pseudo);

	void printS();
	string getName() { return name; }
	bool testReplicatAdresse(sf::IpAddress ip, unsigned short port);

	string findPseudoWithSocket(sf::TcpSocket* socketPtr);
};

class Room_client : public Room
{
protected:
	string pseudo;
	bool hasIdentity;

	mutex msocket;
	thread tReception;
	void fReception();
	bool endReception;

	sf::UdpSocket socket;
public:

	Room_client(string _name);
	~Room_client();
	void print();
	void send(sf::Packet);

	void setIdentity(user identity);
	string getPseudo() { return pseudo; }
};