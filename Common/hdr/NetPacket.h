#pragma once
#include "Packet.h"
#include <mutex>

class NetPacket : public virtual Packet {
public:
	static const unsigned int DGRAM_SIZE = 512;							   // Maximum Datagram size for the Network in Byte
	static const unsigned int HEAD_SIZE = 3;							   // Size of the custom Head
	static const unsigned int DGRAM_SIZE_WO_HEAD = DGRAM_SIZE - HEAD_SIZE; // Maximum Datagram size minus the Head for the Network in Byte

	virtual char* GetDGRAM(unsigned char _DGRAMid);		// Get a pointer to datas' first element of a DGRAM
	virtual size_t GetDGRAMpos(unsigned char _DGRAMid); // Get the position of datas' first element of a DGRAM
};

// Use by User to send Packet throught Network
class SendNetPacket : public NetPacket {
private:
	static unsigned char iPacket; // Used to generate packetID (0 to 255)
	static std::mutex iPacketM;	  // Prevent iPacket from being used in multiple thread at the same time

	void GenerateID(); // Generate a new packetID (0 to 255)
public:
	unsigned char packetID; // ID to recognize the packet and his datas on the Network (0 to 255)


	SendNetPacket(size_t beginCapacity = PacketConst::INIT_PACKET_SIZE); // New instance Constructor, generate a new packet ID
	SendNetPacket(const Packet& base);									 // Copy Constructor, generate a new packet ID
	SendNetPacket(const SendNetPacket& _netPacket);						 // Copy Constructor, didn't generate a new packet ID
};

// Construct by Network, not suppose to be construct in other case (use pointer instead)
class RcvNetPacket : public NetPacket {
private:
	bool* b_rcvDGRAM; // Prevent from receiving multiple times the same DGRAM
public:
	unsigned int clientID;	// IDof the client that send this packet
	unsigned char packetID; // ID to recognize the packet and his datas on the Network (0 to 255)

	unsigned char nbDGRAM;	 // How many Datagram the packet receive
	unsigned char nbDGRAM_T; // How many Datagrams the packet will receive


	RcvNetPacket(unsigned char _packetID, unsigned int _clientID, unsigned char _nbDGRAM_T); //  New instance Constructor, didn't generate a new packet ID
	~RcvNetPacket(); // Destructor

	void AddDGRAM(char* _bytes, size_t _bytesSize); // Add a DGRAM into the NetPacket (only the datas not the head)
};