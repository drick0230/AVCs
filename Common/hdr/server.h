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
	UDP udp;
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
	void fCom();
	
	//fin de thread
	bool endListener;
	bool endCom;

	//méthode utile
	short FindRoomId(string name);

public:
	//std::string myAddress;
	//unsigned short myPort;

	//std::string bindedIP;
	//unsigned short bindedPort;
	std::string defaultGateway;
	std::string publicIP;

	Server(std::string _localNetworkIP, unsigned short port, std::string _defaultGateway = "0", std::string _publicIP = "0");
	Server(unsigned long _ipAddress = INADDR_ANY, unsigned short port = 0, std::string _defaultGateway = "0", std::string _publicIP = "0");
	void analysePacket(Packet& _packet, unsigned int _clientId);
	~Server();

	void print();

	Server& operator=(const Server& _b);
};