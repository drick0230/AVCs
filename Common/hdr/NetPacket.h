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

	void GenerateID() {
		iPacketM.lock();
		iPacket++;
		packetID = iPacket;
		iPacketM.unlock();
	}
public:
	unsigned char packetID; // ID to recognize the packet and his datas on the Network (0 to 255)

	// New instance Constructor, generate a new packet ID
	SendNetPacket(size_t beginCapacity = PacketConst::INIT_PACKET_SIZE) : Packet(beginCapacity) { GenerateID(); }

	// Copy Constructor, generate a new packet ID
	SendNetPacket(const Packet& base) : Packet(base) { GenerateID(); }


	// Copy Constructor, didn't generate a new packet ID
	SendNetPacket(const SendNetPacket& _netPacket) : Packet((Packet)_netPacket), packetID(_netPacket.packetID) {}
};

// Use by Network, not suppose to be use in other case
class RcvNetPacket : public Packet {
public:
	unsigned int clientID; // IDof the client that send this packet
	unsigned char packetID; // ID to recognize the packet and his datas on the Network (0 to 255)

	unsigned char nbRcvDGRAM; // How many Datagram the packet receive
	unsigned char nbRcvDGRAM_T; // How many Datagram the packet need to receive

	bool* b_rcvDGRAM; // Prevent from receiving multiple times the same DGRAM

	//  New instance Constructor, didn't generate a new packet ID
	RcvNetPacket(unsigned char _packetID, unsigned int _clientID, unsigned char _nbRcvDGRAM_T) :
		Packet(_nbRcvDGRAM_T* NetPacket::DGRAM_SIZE_WO_HEAD),
		clientID(_clientID),
		packetID(_packetID),
		nbRcvDGRAM(0),
		nbRcvDGRAM_T(_nbRcvDGRAM_T) {
		emplace(0, capacity());
		b_rcvDGRAM = new bool[_nbRcvDGRAM_T];
	}

	// Destructor
	~RcvNetPacket() {
		delete[] b_rcvDGRAM;
		((Packet*)this)->~Packet();
	}


	void AddDGRAM(char* _bytes, size_t _bytesSize) {
		const unsigned char _packetID = _bytes[0];
		const unsigned char _DGRAMid = _bytes[1];
		const unsigned char _DGRAMsize = _bytes[2];

		if (!b_rcvDGRAM[_DGRAMid]) { // Didn't already receive the DGRAM
			// Mark the DGRAM as received
			b_rcvDGRAM[_DGRAMid] = true;
			nbRcvDGRAM++;

			// Write the datas byte of the DGRAM in the Packet (not the head)
			for (unsigned int _iBytes = NetPacket::HEAD_SIZE, unsigned int _iData = _DGRAMid * NetPacket::DGRAM_SIZE_WO_HEAD; _iBytes < _bytesSize; _iBytes++, _iData++)
				_data[_iData] = _bytes[_iBytes];
		}
	}
};

static unsigned char iPacket = 0;
