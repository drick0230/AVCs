#pragma once
#include <SFML/Network.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

class VOIP {
private:

public:
	// Variables
	sf::SoundBuffer buffer;
	sf::Sound sound;

	sf::UdpSocket socket;

	//unsigned short roomSize;
	//unsigned short maxReceivedPacket;

	sf::IpAddress myIp;
	unsigned short myPort;

	std::vector<sf::IpAddress> theirsIp;
	std::vector<unsigned int> theirsPort;

	std::vector<std::vector<sf::Packet>> theirsPacket;

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
	void TreatAudio();

	void Update();

	unsigned short GetLastElement(sf::IpAddress*);
	unsigned short GetLastElement(sf::Packet*);
};
