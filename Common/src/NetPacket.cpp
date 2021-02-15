/**
 * @file	NetPacket.cpp
 * @author	Dérick Gagnon
 * @date	2021-02-14
 */

#pragma once
#include "NetPacket.h"

#pragma region NetPacket
// Functions
char* NetPacket::GetDGRAM(unsigned char _DGRAMid) {
	return _data + GetDGRAMpos(_DGRAMid);
}

size_t NetPacket::GetDGRAMpos(unsigned char _DGRAMid){
	return (size_t)_DGRAMid * NetPacket::DGRAM_SIZE_WO_HEAD;
}
#pragma endregion


#pragma region SendNetPacket
// Static Variables
unsigned char SendNetPacket::iPacket = 0;
std::mutex SendNetPacket::iPacketM;

// Constructors
SendNetPacket::SendNetPacket(size_t beginCapacity) : Packet(beginCapacity) { GenerateID(); }
SendNetPacket::SendNetPacket(const Packet& base) : Packet(base) { GenerateID(); }
SendNetPacket::SendNetPacket(const SendNetPacket& _netPacket) : Packet((Packet)_netPacket), packetID(_netPacket.packetID) {}

// Functions
void SendNetPacket::GenerateID() {
	SendNetPacket::iPacketM.lock();
	SendNetPacket::iPacket++;
	packetID = SendNetPacket::iPacket;
	SendNetPacket::iPacketM.unlock();
}
#pragma endregion //SendNetPacket

#pragma region RcvNetPacket
// Constructors/Destructors
RcvNetPacket::RcvNetPacket(unsigned char _packetID, unsigned int _clientID, unsigned char _nbDGRAM_T) :
	Packet((size_t)_nbDGRAM_T* NetPacket::DGRAM_SIZE_WO_HEAD),
	clientID(_clientID),
	packetID(_packetID),
	nbDGRAM(0),
	nbDGRAM_T(_nbDGRAM_T) {
	emplace(0, capacity());
	move(0);
	b_rcvDGRAM = new bool[nbDGRAM_T];
	for (unsigned char _i = 0; _i < nbDGRAM_T; _i++)
		b_rcvDGRAM[_i] = false;
}

RcvNetPacket::~RcvNetPacket() {
	delete[] b_rcvDGRAM;
}

// Functions
void RcvNetPacket::AddDGRAM(char* _bytes, size_t _bytesSize) {
	const unsigned char _packetID = _bytes[0];
	const unsigned char _DGRAMid = _bytes[1];
	const unsigned char _nbDGRAM_T = _bytes[2];

	if (!b_rcvDGRAM[_DGRAMid]) { // Didn't already receive the DGRAM
		// Mark the DGRAM as received
		b_rcvDGRAM[_DGRAMid] = true;
		nbDGRAM++;

		char* _toWriteDatas = GetDGRAM(_DGRAMid); // Get the pointer to the first element of the coresponding DGRAM in _data

		// Write the datas byte of the DGRAM in the Packet (not the head)
		for (unsigned int _iData = 0, _iBytes = NetPacket::HEAD_SIZE; _iBytes < _bytesSize; _iData++, _iBytes++)
			_toWriteDatas[_iData] = _bytes[_iBytes];
	}
}
#pragma endregion //RcvNetPacket