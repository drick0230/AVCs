#pragma once
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include<SFML/System/Time.hpp>
#include <iostream>
#include <thread>			// std::this_thread::sleep_for
#include <mutex>
#include <chrono>			// std::chrono::seconds
#include <room.h>
#include "general.h"

class Client
{
protected:
	sf::TcpSocket socket;
	vector<Room_client*> listeRoom;

	//thread
	thread tServerCom;
	//mutex
	mutex mSocket;
	mutex mRoom;
	//thead function
	void fServerCom();
	//end thread
	bool endServerCom;

	//méthode utile
	short FindRoomId(string name);


public:
	Client(sf::IpAddress ip, unsigned short port);
	~Client();

	void analysePacket(sf::Packet packet);

	bool joinRoom(string roomName, string pseudo);
	bool createRoom(string roomName);
	bool exitRoom(string roomName);

	void print();
	void send(string room, sf::Packet packet);
};