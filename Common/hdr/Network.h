#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "NetPacket.h"
#include "general.h"

namespace ProtocoleTypes {
	enum PROTOCOLE_TYPES { TCP, UDP, BOTH };
}

#pragma region Protocole
//class Protocole {
//protected:
//	struct addrinfo clientInfo;
//	struct addrinfo serverInfo;
//
//	Protocole(int _family, int _sockType, int _protocol);
//
//	virtual void tcpListen() {};
//	virtual int udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength) { return -2; };
//	virtual int udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength) { return -2; };
//	virtual int udpTcpSend(sockaddr_in _sendToAddr, char* _bufferToSend, const int _bufferToSendLength) { return -2; };
//
//	void Send(SOCKET _clientSocket, std::string _ipAddress, unsigned short _port, char* _bufferToSend, const int _bufferToSendLength);
//public:
//	std::vector<std::string> addressBook;
//	std::vector<unsigned short> portBook;
//
//	unsigned int protocoleType;
//
//	~Protocole();
//	SOCKET mySocket;
//	
//	// TCP : Receive packets from a specific Client
//	// UDP : Receive packets from everyone. Return the ID of the sender.
//	unsigned int WaitReceive(Packet& _recvPacket, unsigned int _clientID = -1);
//
//	void Send(std::string _str);
//	void Send(unsigned int _clientID, std::string _str);
//	void Send(Packet& _packetToSend);
//	void Send(unsigned int _clientID, Packet& _packetToSend);
//	void Send(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength);
//
//	virtual std::string GetClientInfo(unsigned short& _returnPort, unsigned int _clientID) { return "ERROR"; };
//};

class UDP {
private:
	SOCKET mySocket;
	struct sockaddr_in serverAddr;
	std::vector<sockaddr_in> sockAddressBook;

	// NetPacket Buffer
	static const size_t netPacketBufferSize = 10;
	NetPacket* netPacketBuffer[netPacketBufferSize];

	size_t first; // Represent the first element
	size_t end; // Represent the end of the buffer ( last element + 1 )

	std::thread tReceiving;
	bool isReceiving;

	void Send(unsigned int _clientID, NetPacket& _netPacket);

	bool IsInNetPacketBuffer(unsigned int _clientID, unsigned int _packetID);
	void Emplace_back(unsigned int _packetID, unsigned int _clientID, size_t _packetSize);

	// Return the first NetPacket if it receive all his datagram or return NULL (The caller must manually delete it)
	NetPacket* Pop_front();

	void MoveFirst(); // Move first by 1 and overlap it at netPacketBufferSize
	void MoveEnd(); // Move last by 1 and overlap it at netPacketBufferSize

public:
	std::vector<std::string> addressBook; // Address of the clients
	std::vector<unsigned short> portBook; // Ports of the clients

	UDP();
	~UDP();

	bool Bind(std::string _ipAddress, unsigned short _port);
	bool Bind(unsigned long _ipAddress, unsigned short _port);

	bool IsInBook(std::string _ipAddress, unsigned short _port);
	unsigned int AddToBook(std::string _ipAddress, unsigned short _port);

	// Receive packets from everyone.
	// Return the ID of the sender and the received NetPacket.
	void BeginReceiving();
	NetPacket* GetNetPacket();	// Return the first NetPacket if it receive all his datagram or return NULL (The caller must manually delete it)

	void Send(unsigned int _clientID, NetPacket& _netPacket);

	std::string GetClientInfo(unsigned short& _returnPort, unsigned int _clientID);
};
#pragma endregion

class Network {
private:


public:
	// Public Variables
	static int hr;

	static std::vector<UDP> udp;

	// Public Functions
	static void Initialize();
	static void Destruct();

	static void Add(const unsigned int _protocoleType, const unsigned char _nbToAdd = 1);

	static bool Compare(sockaddr_in _a, sockaddr_in _b);

	static sockaddr_in CreateSockaddr_in(unsigned short _family, std::string _ipAddress, unsigned short _port);
	static sockaddr_in Network::CreateSockaddr_in(unsigned short _family, unsigned long _ipAddress, unsigned short _port);

	static std::string GetSocketInfo(unsigned short& _returnPort, SOCKET _socket);
	static std::string GetSocketInfo(unsigned short& _returnPort, sockaddr_in _socket);
};