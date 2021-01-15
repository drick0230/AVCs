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

class UDP {
private:
	std::mutex inUse;

	SOCKET mySocket;
	struct sockaddr_in serverAddr;
	std::vector<sockaddr_in> sockAddressBook;

	// NetPacket Buffer
	static const size_t netPacketBufferSize = 10;
	RcvNetPacket* netPacketBuffer[netPacketBufferSize];

	size_t first; // Represent the first element
	size_t end; // Represent the end of the buffer ( last element + 1 )

	std::thread tReceiving;
	std::mutex isReceiving; // Prevent tReceiving from being call multiple times and stop his execution

	bool IsEmpty(); // Return true if the buffer is empty
	size_t UDP::FoundNetPacket(unsigned int _clientID, unsigned int _packetID); // If the packet is in the buffer, Return his position in the buffer. In other case, return the size of the buffer.
	void UDP::Emplace_back(unsigned int _packetID, unsigned int _clientID, unsigned char _DGRAMSize_T);

	RcvNetPacket* UDP::Pop_front();	// Return the first NetPacket, if it receive all his datagram, or return NULL (The caller must manually delete it)

	void MoveFirst(); // Move first by 1 and overlap it at netPacketBufferSize
	void MoveEnd(); // Move end by 1 and overlap it at netPacketBufferSize
	size_t UDP::GetLast(); // Get the last element

	void UDP::Send(unsigned int _clientID, char* _bytes, const size_t _bytesSize, const unsigned char _packetID, const unsigned char _DGRAMid, const unsigned char _nbDGRAM_T); // Send a DGRAM with a head. _bytesSize should not exceed NetPacket::DGRAM_SIZE_WO_HEAD

public:
	std::vector<std::string> addressBook; // Address of the clients
	std::vector<unsigned short> portBook; // Ports of the clients

	UDP();
	UDP(UDP&& _udp);
	~UDP();

	bool Bind(std::string _ipAddress, unsigned short _port); // Bind to an IPV4 address represent by a string 0.0.0.0 format and a port
	bool Bind(unsigned long _ipAddress, unsigned short _port); // Bind to an IPV4 address represent by a ulong and a port. Ex: 	INADDR_ANY and 0 for an auto ip and port

	bool IsInBook(std::string _ipAddress, unsigned short _port);
	unsigned int AddToBook(std::string _ipAddress, unsigned short _port);

	void BeginReceiving(); // Start Async operation to receive packets from everyone and store them

	RcvNetPacket* GetNetPacket(); // Return the first NetPacket if it receive all his datagram or return NULL (The caller must manually delete it)
	void Send(unsigned int _clientID, SendNetPacket& _netPacket); // Send the SendNetPacket to a client
};

class Network {
private:


public:
	// Public Variables
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