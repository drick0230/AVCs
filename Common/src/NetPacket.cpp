#pragma once
#include "NetPacket.h"

// SendNetPacket
// Static Variables
static unsigned char iPacket = 0;
std::mutex SendNetPacket::iPacketM;

SendNetPacket::SendNetPacket(size_t beginCapacity) : Packet(beginCapacity) { GenerateID(); }
SendNetPacket::SendNetPacket(const Packet& base) : Packet(base) { GenerateID(); }
SendNetPacket::SendNetPacket(const SendNetPacket& _netPacket) : Packet((Packet)_netPacket), packetID(_netPacket.packetID) {}

void SendNetPacket::GenerateID() {
	SendNetPacket::iPacketM.lock();
	SendNetPacket::iPacket++;
	packetID = SendNetPacket::iPacket;
	SendNetPacket::iPacketM.unlock();
}

// RcvNetPacket
RcvNetPacket::RcvNetPacket(unsigned char _packetID, unsigned int _clientID, unsigned char _nbRcvDGRAM_T) :
	Packet((size_t)_nbRcvDGRAM_T * NetPacket::DGRAM_SIZE_WO_HEAD),
	clientID(_clientID),
	packetID(_packetID),
	nbRcvDGRAM(0),
	nbRcvDGRAM_T(_nbRcvDGRAM_T) {
	emplace(0, capacity());
	b_rcvDGRAM = new bool[_nbRcvDGRAM_T];
}

RcvNetPacket::~RcvNetPacket() {
	delete[] b_rcvDGRAM;
}

void RcvNetPacket::AddDGRAM(char* _bytes, size_t _bytesSize) {
	const unsigned char _packetID = _bytes[0];
	const unsigned char _DGRAMid = _bytes[1];
	const unsigned char _DGRAMsize = _bytes[2];

	if (!b_rcvDGRAM[_DGRAMid]) { // Didn't already receive the DGRAM
		// Mark the DGRAM as received
		b_rcvDGRAM[_DGRAMid] = true;
		nbRcvDGRAM++;

		// Write the datas byte of the DGRAM in the Packet (not the head)
		for (unsigned int _iBytes = NetPacket::HEAD_SIZE, _iData = _DGRAMid * NetPacket::DGRAM_SIZE_WO_HEAD; _iBytes < _bytesSize; _iBytes++, _iData++)
			_data[_iData] = _bytes[_iBytes];
	}
}