#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <iostream>
#include <vector>

#include "Packet.h"
#include "general.h"

namespace ProtocoleTypes {
	enum PROTOCOLE_TYPES { TCP, UDP, BOTH };
}

#pragma region Protocole
class Protocole {
protected:
	int hr;

	struct addrinfo clientInfo;
	struct addrinfo serverInfo;

	Protocole(int _family, int _sockType, int _protocol);

	virtual void tcpListen() {};
	virtual int udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength) { return -2; };
	virtual int udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength) { return -2; };
	virtual int udpTcpSend(sockaddr_in _sendToAddr, char* _bufferToSend, const int _bufferToSendLength) { return -2; };

	void Send(SOCKET _clientSocket, std::string _ipAddress, unsigned short _port, char* _bufferToSend, const int _bufferToSendLength);
public:
	unsigned int protocoleType;

	~Protocole();
	SOCKET mySocket;

	virtual unsigned int Connect(std::string _ipAddress, unsigned short _port) { return false; };

	virtual unsigned int WaitClientConnection() { std::cerr << "Only for TCP protocole\n"; throw  "Only for TCP protocole\n"; return -2; };

	// TCP : Receive packets from a specific Client
	// UDP : Receive packets from everyone. Return the ID of the sender.
	unsigned int WaitReceive(Packet& _recvPacket, unsigned int _clientID = -1);

	void Send(std::string _str);
	void Send(unsigned int _clientID, std::string _str);
	void Send(Packet& _packetToSend);
	void Send(unsigned int _clientID, Packet& _packetToSend);
	void Send(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength);

	virtual std::string GetClientInfo(unsigned short& _returnPort, unsigned int _clientID) { return "ERROR"; };
};

class TCP : public Protocole {
private:
	std::vector<SOCKET> clientsSocket;

	void tcpListen();

	int udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength);
	int udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength);
public:
	TCP();

	bool Host(unsigned short _port);
	bool Host(std::string _port);

	unsigned int WaitClientConnection();
	unsigned int Connect(std::string _ipAddress, unsigned short _port);

	std::string GetClientInfo(unsigned short& _returnPort, unsigned int _clientID);
};

class UDP : public Protocole {
private:
	struct sockaddr_in serverAddr;

	std::vector<sockaddr_in> addressBook;
	std::vector<int> addressLengthBook;

	int udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength);
	int udpTcpSend(sockaddr_in _sendToAddr, char* _bufferToSend, const int _bufferToSendLength);

	int udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength);
public:
	UDP();

	bool Bind(std::string _ipAddress, unsigned short _port);
	unsigned int Connect(std::string _ipAddress, unsigned short _port);

	std::string GetClientInfo(unsigned short& _returnPort, unsigned int _clientID);
};
#pragma endregion

class Network {
private:


public:
	// Windows API
	static int hr;

	static std::vector<TCP> tcp;
	static std::vector<UDP> udp;
	// Public Variables

	//// Constructors
	//Network();
	//// Destructors
	//~Network();

	// Public Functions
	static void Initialize();
	static void Destruct();

	static void Add(const unsigned int _protocoleType, const unsigned char _nbToAdd = 1);

	static bool Compare(sockaddr_in _a, sockaddr_in _b);
	static sockaddr_in CreateSockaddr_in(unsigned short _family, std::string _ipAddress, unsigned short _port);
};