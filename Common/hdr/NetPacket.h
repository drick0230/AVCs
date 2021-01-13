#pragma once
#include "Packet.h"
#include <mutex>

namespace NetPacket {
	static const unsigned int DGRAM_SIZE = 512; // Maximum Datagram size for the Network in Byte
	static const unsigned int HEAD_SIZE = 3; // Size of the custom Head
	static const unsigned int DGRAM_SIZE_WO_HEAD = DGRAM_SIZE - HEAD_SIZE; // Maximum Datagram size minus the Head for the Network in Byte
}

// Use by User to send Packet throught Network
class SendNetPacket : public Packet {
private:
	static unsigned char iPacket;
	static std::mutex iPacketM;

	void GenerateID();
public:
	unsigned char packetID; // ID to recognize the packet and his datas on the Network (0 to 255)

	// New instance Constructor, generate a new packet ID
	SendNetPacket(size_t beginCapacity = PacketConst::INIT_PACKET_SIZE);

	// Copy Constructor, generate a new packet ID
	SendNetPacket(const Packet& base);


	// Copy Constructor, didn't generate a new packet ID
	SendNetPacket(const SendNetPacket& _netPacket);
};

// Construct by Network, not suppose to be construct in other case (use pointer instead)
class RcvNetPacket : public Packet {
private:
	bool* b_rcvDGRAM; // Prevent from receiving multiple times the same DGRAM
public:
	unsigned int clientID; // IDof the client that send this packet
	unsigned char packetID; // ID to recognize the packet and his datas on the Network (0 to 255)

	unsigned char nbRcvDGRAM; // How many Datagram the packet receive
	unsigned char nbRcvDGRAM_T; // How many Datagram the packet need to receive

	//  New instance Constructor, didn't generate a new packet ID
	RcvNetPacket(unsigned char _packetID, unsigned int _clientID, unsigned char _nbRcvDGRAM_T);

	// Destructor
	~RcvNetPacket();

	void AddDGRAM(char* _bytes, size_t _bytesSize);
};