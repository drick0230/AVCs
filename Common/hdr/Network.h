/**
 * @file	Network.h
 * @author	Dérick Gagnon
 * @date	2021-02-14
 */

#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h> // inet_pton

#include <string>
#include <iostream>
#include <vector>
#include <queue>

#include "NetPacket.h"
#include "general.h"

namespace ProtocoleTypes {
	enum PROTOCOLE_TYPES { TCP, UDP, BOTH };
}

struct NetworkInterface {
	// IPV4 of the ethernet interface
	std::string ip;

	// IPV4 as unsigned long of the ethernet interface
	unsigned long ip_ulong;

	// IPV4 Subnet Mask of the ethernet interface
	std::string mask;

	// IPV4 Subnet Mask as unsigned long of the ethernet interface
	unsigned long mask_ulong;

	// IPV4 Default Gateway of the ethernet interface
	std::string gateway;

	// IPV4 Default Gateway as unsigned long of the ethernet interface
	unsigned long gateway_ulong;
};

class UDP {
private:
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
	// Prevent the UDP from being used in other threads
	std::mutex inUse;

	// Network Interface used by the socket
	struct NetworkInterface netInterface;

	// Address of the clients
	std::vector<std::string> addressBook;

	// Ports of the clients
	std::vector<unsigned short> portBook;

	UDP();
	UDP(UDP&& _udp);
	~UDP();

	// Bind to the IPV4 address of an interface, a port and set the interface as the Network Interface for the socket
	bool Bind(NetworkInterface _netInterface, unsigned short _port);

	// Bind to an IPV4 address represent by a string 0.0.0.0 format and a port
	bool Bind(std::string _ipAddress, unsigned short _port);

	// Bind to an IPV4 address represent by a ulong and a port. Ex: 	INADDR_ANY and 0 for an auto ip and port
	bool Bind(unsigned long _ipAddress, unsigned short _port);


	// Return true if the corresponding IP and Port are in book. Lock inUse by default
	bool IsInBook(std::string _ipAddress, unsigned short _port);

	// Return the position in book of the corresponding IP and Port or the size of the book.
	unsigned int PosInBook(std::string _ipAddress, unsigned short _port);

	// Add the IP and Port to the book if not already in. Return the ClientID of the added one or -1.
	unsigned int AddToBook(std::string _ipAddress, unsigned short _port);


	// Generate the ethernet interface info. Must be used after sending or receiving packets
	// bool GenerateEthInfo();

	//// Return the IPV4 of the ethernet interface used by the socket
	//std::string ip() { return ip_; };

	//// Return the IPV4 Subnet Mask of the ethernet interface used by the socket
	//std::string mask() { return mask_; };

	//// Return the IPV4 Default Gateway of the ethernet interface used by the socket
	//std::string gateway() { return gateway_; };


	// Start Async operation to receive packets from everyone and store them
	void BeginReceiving();


	// Return the first NetPacket if it receive all his datagram or return NULL (The caller must manually delete it)
	RcvNetPacket* GetNetPacket();

	// Send the SendNetPacket to a client
	void Send(unsigned int _clientID, SendNetPacket& _netPacket);
};

class Network {
private:

public:
	// Public Variables
	static std::vector<UDP> udp;
	static std::vector<struct NetworkInterface> netInterfaces;

	// Public Functions
	static void Initialize();
	static void Destruct();

	static void EnumerateInterfaces();

	static void Add(const unsigned int _protocoleType, const unsigned char _nbToAdd = 1);

	static bool Compare(sockaddr_in _a, sockaddr_in _b);

	static sockaddr_in CreateSockaddr_in(unsigned short _family, std::string _ipAddress, unsigned short _port);
	static sockaddr_in Network::CreateSockaddr_in(unsigned short _family, unsigned long _ipAddress, unsigned short _port);

	static std::string GetSocketInfo(unsigned short& _returnPort, SOCKET _socket);
	static std::string GetSocketInfo(unsigned short& _returnPort, sockaddr_in _socket);
};