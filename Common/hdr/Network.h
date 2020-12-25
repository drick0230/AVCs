#pragma once
// Network API
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <string>
#include <iostream>
#include <vector>

namespace ProtocoleTypes {
	enum PROTOCOLE_TYPES { TCP, UDP, BOTH };
}

// Class Prototype
class Protocole {
protected:
	struct addrinfo clientInfo;
	struct addrinfo serverInfo;

	Protocole(int _family, int _sockType, int _protocol);

	virtual void tcpListen() {};
	virtual int udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength) { return -2; };
	virtual int udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength) { return -2; };
	virtual int udpTcpSend(sockaddr_in _sendToAddr, char* _bufferToSend, const int _bufferToSendLength) { return -2; };
public:
	unsigned int protocoleType;

	~Protocole();
	SOCKET connectSocket;
	SOCKET hostSocket;

	virtual bool Connect(std::string _ipAddress, unsigned short _port, bool _openPort = false) { return false; };

	virtual void WaitClientConnection() { std::cerr << "Only for TCP protocole\n"; throw  "Only for TCP protocole\n"; };
	unsigned int WaitReceive(unsigned int _clientID = -1);

	void Send(std::string _str);
	void Send(unsigned int _clientID, std::string _str);
	void Send(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength);
};

class TCP : public Protocole {
private:
	std::vector<SOCKET> clientsSocket;

	void tcpListen();

	int udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength);
	int udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength);
public:
	TCP();

	bool Host(unsigned short _port, bool _openPort = false);
	bool Host(std::string _port, bool _openPort = false);

	void WaitClientConnection();
	bool Connect(std::string _ipAddress, unsigned short _port, bool _openPort = false);
};

class UDP : public Protocole {
private:
	//std::vector<sockaddr_in> clientsAddress;
	//std::vector<int> clientsAddressSize;
	struct sockaddr_in serverAddr;

	std::vector<sockaddr_in> addressBook;
	std::vector<int> addressLengthBook;

	int udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength);
	int udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength);
	int udpTcpSend(sockaddr_in _sendToAddr, char* _bufferToSend, const int _bufferToSendLength);
public:
	UDP();

	bool Bind(std::string _ipAddress, unsigned short _port);
	bool Connect(std::string _ipAddress, unsigned short _port, bool _openPort = false);
};


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

	static void Add(const unsigned int _protocoleType);

	static bool Compare(sockaddr_in _a, sockaddr_in _b);
	static sockaddr_in CreateSockaddr_in(unsigned short _family, std::string _ipAddress, unsigned short _port);
};