#pragma once
#include <SFML/Network.hpp>
#include <iostream>

class VOIP {
private:

public:
	// Variables
	sf::UdpSocket socket;

	unsigned short roomSize;
	unsigned short maxReceivedPacket;

	sf::IpAddress myIp;
	unsigned short myPort;

	std::vector<sf::IpAddress> theirsIp;
	std::vector<unsigned int> theirsPort;

	sf::Packet** theirsPacket;

	inline int nbUser() { return theirsIp.size(); }


	// Constructors
	//VOIP();
	VOIP(std::string, unsigned short);

	// Destructors
	~VOIP();

	// Functions
	void Connect(sf::IpAddress, unsigned int);
	void Host();
	void Broadcast(sf::Packet*);
	void Receive();
	void Treat();

	void Update();

	unsigned short GetLastElement(sf::IpAddress*);
	unsigned short GetLastElement(sf::Packet*);
};
