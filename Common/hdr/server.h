#pragma once
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>			// std::this_thread::sleep_for
#include <mutex>
#include <chrono>			// std::chrono::seconds
#include <room.h>
#include "general.h"

struct TCPServerConnection
{
	sf::TcpSocket socket;
	unsigned short state = 0; //0 = disconnected, 1 = connected
};

class Server
{
protected:
	vector<TCPServerConnection*> listeConnection;
	vector<Room_server> listeRoom;
	sf::TcpListener listener;
	//mutex toujours bloquer dans cet ordre
	mutex mListener;
	mutex mCom;
	mutex mRoom;

	//thread
	thread tListener;
	thread tCom;
	//threadFunction
	void fListener(unsigned short port);
	void fCom();
	//fin de thread
	bool endListener;
	bool endCom;

public:
	Server(unsigned short port);
	void analysePacket(sf::Packet packet, int id);
	~Server();

	void print();

};