#pragma once
#include "Packet.h"
#include <mutex>

class NetPacket : public Packet {
private:
	static const unsigned int maxSize = 512;
	static unsigned char iPacket;
	static std::mutex iPacketM;
public:
	unsigned char packetID;

	NetPacket(size_t beginCapacity = PacketConst::INIT_PACKET_SIZE) : Packet(beginCapacity) {
		iPacketM.lock();
		iPacket++;
		packetID = iPacket;
		iPacketM.unlock();
	}
};

static unsigned char iPacket = 0;
