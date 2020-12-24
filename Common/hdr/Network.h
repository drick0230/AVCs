#pragma once
// Network API
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <iostream>
#include <vector>

// Class Prototype
class Protocole {
protected:
	struct addrinfo clientInfo;
	struct addrinfo serverInfo;

	Protocole(int _family, int _sockType, int _protocol);
public:
	~Protocole();
	SOCKET connectSocket;
	SOCKET hostSocket;

	std::vector<SOCKET> clientsSocket;

	bool Connect(std::string _ipAddress, std::string _port, bool _openPort = false);

	bool Host(short _port, bool _openPort = false);
	bool Host(std::string _port, bool _openPort = false);

	void WaitClientConnection();
	void WaitReceive(unsigned int _clientID = -1);

	void Send(std::string _str);
	void Send(unsigned int _clientID, std::string _str);

};

class TCP : public Protocole {
public:
	TCP();
};

class UDP : public Protocole {
public:
	UDP();
};


class Network {
private:


public:
	// Windows API
	static int hr;

	static TCP tcp;
	static UDP udp;
	// Public Variables

	//// Constructors
	//Network();
	//// Destructors
	//~Network();

	// Public Functions
	static void Initialize();
	static void Destruct();
};