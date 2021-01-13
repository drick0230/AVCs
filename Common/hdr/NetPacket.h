#pragma once
#include "Packet.h"
#include <mutex>

class NetPacket : public Packet {
private:
	static unsigned char iPacket;
	static std::mutex iPacketM;

	void GenerateID() {
		iPacketM.lock();
		iPacket++;
		packetID = iPacket;
		iPacketM.unlock();
	}
public:
	static const unsigned int maxDGRAMSize = 512; // Maximum Datagram size for the Network in Byte

	unsigned int clientID; // IDof the client that send this packet
	unsigned char packetID; // ID to recognize the packet and his datas on the Network (0 to 255)
	unsigned char rcvDGRAM; // How many Datagram the packet receive
	unsigned char rcvDGRAMSize; // How many Datagram the packet need to receive

	// New instance Constructor, generate a new packet ID
	NetPacket(size_t beginCapacity = PacketConst::INIT_PACKET_SIZE) : Packet(beginCapacity) { GenerateID(); }

	// Copy Constructor, generate a new packet ID
	NetPacket(const Packet& base) : Packet(base) { GenerateID(); }

	//  New instance Constructor, didn't generate a new packet ID
	NetPacket(unsigned char _packetID, unsigned int _clientID, size_t beginCapacity = PacketConst::INIT_PACKET_SIZE) : Packet(beginCapacity), clientID(_clientID), packetID(_packetID) {}

	// Copy Constructor, didn't generate a new packet ID
	NetPacket(const NetPacket& _netPacket) : Packet((Packet)_netPacket), clientID(_netPacket.clientID), packetID(_netPacket.packetID) {}
};

static unsigned char iPacket = 0;
