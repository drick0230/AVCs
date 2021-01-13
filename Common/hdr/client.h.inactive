#pragma once
#include "room.h"

class Client
{
protected:
	//sf::TcpSocket socket;
	TCP tcp;
	UDP udp;
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
	Client(std::string _ipAddress, unsigned short _port);
	~Client();

	void analysePacket(Packet _packet);

	bool joinRoom(string roomName, string pseudo);
	bool createRoom(string roomName);
	bool exitRoom(string roomName);

	void print();
	void send(string room, Packet _packet);
};