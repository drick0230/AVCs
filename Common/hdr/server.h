#pragma once
#include "room.h"

struct TCPServerConnection
{
	//sf::TcpSocket socket;
	unsigned short state = 0; //0 = disconnected, 1 = connected
};

class Server
{
protected:
	vector<TCPServerConnection*> listeConnection;
	vector<Room_server> listeRoom;
	TCP tcp;
	//sf::TcpListener listener;
	//mutex toujours bloquer dans cet ordre
	mutex mListener;
	mutex mCom;
	mutex mRoom;

	//thread
	thread tListener;
	thread tCom;
	std::vector<std::thread> clientsThreads;
	//threadFunction
	void fListener();
	void fCom(unsigned int _clientID);
	//fin de thread
	bool endListener;
	bool endCom;

	//m�thode utile
	short FindRoomId(string name);

public:
	Server(unsigned short port);
	void analysePacket(Packet& _packet, unsigned int _clientId);
	~Server();

	void print();

};