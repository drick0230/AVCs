#pragma once
#include "room.h"

class Client
{
protected:
	//sf::TcpSocket socket;
	UDP udp;
	vector<Room_client*> listeRoom;

	//thread
	thread tServerCom;
	std::vector<std::thread> tKeepAlives;
	//mutex
	mutex mSocket;
	mutex mRoom;
	//thead function
	void fServerCom();
	void KeepAlive(unsigned int _clientID, unsigned int _ms);

	//end thread
	bool endServerCom;

	//méthode utile
	short FindRoomId(string name);

public:
	Client(std::string _serverIP, unsigned short _serverPort);
	~Client();

	void analysePacket(Packet _packet);

	bool joinRoom(string roomName, string pseudo);
	bool createRoom(string roomName);
	bool exitRoom(string roomName);

	void print();
	void send(string room, Packet _packet);
};