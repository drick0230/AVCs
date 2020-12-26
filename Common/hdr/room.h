#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <thread>			// std::this_thread::sleep_for
#include <mutex>
#include <chrono>			// std::chrono::seconds
#include "general.h"
#include "Network.h"

using namespace std;

struct user
{
	string			pseudo = "default";
	unsigned int id;
	//sf::IpAddress	ip = sf::IpAddress::Any;
	//unsigned short	port = 0;
};

//struct userSocket
//{
//	//sf::TcpSocket* socketPtr = NULL;
//	string pseudo;
//};

class Room
{
protected:
	vector <user> listeUser;
public:
	string name;

	Room(string _name);
	void print();
	virtual void addUser(user new_user);
	void addUser(string user_pseudo, unsigned int _id);

	bool addUserAdaptative(user new_user);
	bool addUserAdaptative(string user_pseudo, unsigned int _id);

	void removeUser(string user_pseudo);

};

class Room_server : public Room
{
protected:
public:
	TCP* pTCP;

	Room_server(string _name);

	void addUser(user new_user);
	void removeUser(string user_pseudo);

	void printS();
	string getName() { return name; }
	bool testReplicatAdresse(unsigned int _id);

	string findPseudoWithSocket(unsigned int _id);
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

	std::thread tListen;
	//sf::UdpSocket socket;
public:
	UDP udp;

	Room_client(string _name);
	~Room_client();
	void print();
	void send(Packet);

	void setIdentity(std::string _ipAddress, unsigned short _port);
	string getPseudo() { return pseudo; }
};