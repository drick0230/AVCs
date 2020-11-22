#include "VOIP.h"

// Constructors
VOIP::VOIP(std::string _myIp, unsigned short _myPort) : myIp(_myIp), myPort(_myPort){
	roomSize = 10;
	maxReceivedPacket = 10;

	// Initialize theirsIp and theirsPort at length of maxUsers and their elements at '\0'
	// Initialise theirsPackets at length of [maxUsers][maxPackets] and clear his elements 
	unsigned short _maxUsers = roomSize + 1;
	unsigned short _maxReceivedPacket = maxReceivedPacket + 1;

	theirsIp.clear();
	theirsPort.clear();
	theirsPacket = new sf::Packet * [_maxUsers];
	for (unsigned short i = 0; i < _maxUsers; i++)
	{
	    theirsPacket[i] = new sf::Packet[_maxReceivedPacket];
		for (unsigned short i2 = 0; i2 < _maxReceivedPacket; i2++)
			theirsPacket[i][i2].clear();
	}

	Host();
}

// Destructors
VOIP::~VOIP() {
	unsigned short _maxUsers = roomSize + 1;
	unsigned short _maxReceivedPacket = maxReceivedPacket + 1;

	for (unsigned short i = 0; i < _maxUsers; i++)
	{
		delete theirsPacket[i];
	}

	delete theirsPacket;
}

void VOIP::Connect(sf::IpAddress _hisIp, unsigned int _hisPort) {
	theirsIp.push_back(_hisIp);
	theirsPort.push_back(_hisPort);
}

void VOIP::Host() {
	socket.bind(myPort); // Bind myPort to the socket / The UDP Socket will listen on that port
}

void VOIP::Broadcast(sf::Packet* _packet) {
	// Broadcast _packet to all connected users (Theirs)
	for(unsigned int _i = 0; _i < nbUser(); _i++){
		if (socket.send(*_packet, theirsIp[_i], theirsPort[_i]) != sf::Socket::Done) {
			std::cout << "Error at <void VOIP::Broadcast(sf::Packet* _packet)> with user" << theirsIp[_i] << ' ' << theirsPort[_i] << '\n';
		}
	}
}

void VOIP::Receive() {
	sf::Packet _receivedPacket;

	// Receive _packet from all connected users (Theirs)
	for (unsigned int _i = 0; _i < nbUser(); _i++) {
		sf::IpAddress _sendIp; // The sender IP Adress
		unsigned short _sendPort;	// The sender Port

		if (socket.receive(_receivedPacket, _sendIp, _sendPort) != sf::Socket::Done) {
			// Error
			std::cout << "Error at <void VOIP::Receive()> with user " << theirsIp[_i] << ' ' << theirsPort[_i] << '\n';
		}
		else if (_sendIp == theirsIp[_i] && _sendPort == theirsPort[_i]) {
			// If received something, add the packet at end of theirsPacket
			if (_receivedPacket.getDataSize() > 0) {
				unsigned short _lastElement = GetLastElement(theirsPacket[_i]);

				theirsPacket[_i][_lastElement] = _receivedPacket;
			}
		}
		else {
			// The sender is sus
			std::cout << "Error at <void VOIP::Receive()> with user " << _sendIp << ' ' << _sendPort << " is not " << theirsIp[_i] << ' ' << theirsPort[_i] << '\n';
		}

	}
}

void VOIP::Treat() {
	sf::Packet _receivedPacket;

	// Receive _packet from all connected users (Theirs)
	for (unsigned int _i = 0; _i < nbUser(); _i++) {
		unsigned short _i2 = 0;
		while (theirsPacket[_i][_i2].getDataSize() > 0) {
			std::string _msg;
			theirsPacket[_i][_i2] >> _msg;
			std::cout << _msg << '\n';
		}
	}
}

void VOIP::Update() {
	// Broadcast my voice to connected users (Theirs)
	sf::Packet _packet;
	_packet << "Hello VOIP!!!";
	Broadcast(&_packet);

	// Receive connected users' (Theirs) voice
	Receive();

	// Treat connected users' (Theirs) voice
}

unsigned short VOIP::GetLastElement(sf::IpAddress* _ipAdress) {
	// Get the last element of the Array of sf::IpAdress
	unsigned short _lastElement = 0;
	while (_ipAdress[_lastElement] != '\0')
		_lastElement++;

	return _lastElement;
}

unsigned short VOIP::GetLastElement(sf::Packet* _packets) {
	// Get the last element of the Array of sf::Packet
	unsigned short _lastElement = 0;
	while (_packets[_lastElement].getDataSize() != 0)
		_lastElement++;

	return _lastElement;
}