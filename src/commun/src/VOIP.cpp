#include "VOIP.h"

// Constructors
VOIP::VOIP(std::string _myIp, unsigned short _myPort) : myIp(_myIp), myPort(_myPort){
	theirsIp.clear();
	theirsPort.clear();
	theirsPacket.clear();

	Host();
}

// Destructors
VOIP::~VOIP() {}

void VOIP::Connect(sf::IpAddress _hisIp, unsigned int _hisPort) {
	theirsIp.push_back(_hisIp);
	theirsPort.push_back(_hisPort);
	theirsPacket.emplace_back();
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

void VOIP::BroadcastVoice() {
	const sf::SoundBuffer& _soundBuffer = voiceRecorder.getBuffer();

	sf::Uint64 _sampleCount = _soundBuffer.getSampleCount();
	unsigned int _channelCount = _soundBuffer.getChannelCount();
	unsigned int _sampleRate = _soundBuffer.getSampleRate();

	sf::Packet _packet;

	_packet << _sampleCount;
	_packet << _channelCount;
	_packet << _sampleRate;

	for (unsigned int i = 0; i < _soundBuffer.getSampleCount(); i++)
	{
		sf::Int16 _sample = _soundBuffer.getSamples()[i];
		_packet << _sample;
	}

	Broadcast(&_packet);
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
				theirsPacket[_i].push_back(_receivedPacket);
			}
		}
		else {
			// The sender is sus
			std::cout << "Error at <void VOIP::Receive()> with user " << _sendIp << ' ' << _sendPort << " is not " << theirsIp[_i] << ' ' << theirsPort[_i] << '\n';
		}

	}
}

void VOIP::Treat() {
	// Receive _packet from all connected users (Theirs)
	for (unsigned int _i = 0; _i < nbUser(); _i++) {
		unsigned short _i2 = 0;
		while (theirsPacket[_i][_i2].getDataSize() > 0) {
			std::string _msg;
			theirsPacket[_i][_i2] >> _msg;
			std::cout << _msg << '\n';
			theirsPacket[_i].erase(theirsPacket[_i].begin() + _i2);
		}
	}
}

void VOIP::TreatAudio() {
	// Receive _packet from all connected users (Theirs)
	for (unsigned int _i = 0; _i < nbUser(); _i++) {
		for (unsigned int _i2 = 0; _i2 < theirsPacket[_i].size(); _i2++)
		{
			sf::Uint64 _sampleCount = 0;
			unsigned int _channelCount = 0;
			unsigned int _sampleRate = 0;

			theirsPacket[_i][_i2] >> _sampleCount;
			theirsPacket[_i][_i2] >> _channelCount;
			theirsPacket[_i][_i2] >> _sampleRate;

			std::vector<sf::Int16> _samples(_sampleCount);

			for (unsigned int i = 0; i < _sampleCount; i++)
			{
				sf::Int16 _sample;
				theirsPacket[_i][_i2] >> _sample;
				_samples[i] = _sample;
				//_samples.push_back(_sample);
			}

			std::cout << "_sampleCount <" << _sampleCount << "> _channelCount <" << _channelCount << "> _sampleRate <" << _sampleRate << "> \n";
			soundBuffer.loadFromSamples(&_samples[0], _sampleCount, _channelCount, _sampleRate);
			sound.setBuffer(soundBuffer);
			sound.play();

			theirsPacket[_i].erase(theirsPacket[_i].begin() + _i2);
		}
	}
}

void VOIP::Update() {
	// Broadcast my voice to connected users (Theirs)
	Send();


	// Receive connected users' (Theirs) voice
	Receive();

	// Treat connected users' (Theirs) voice
	TreatAudio();
}

void VOIP::Record() {
	voiceRecorder.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	voiceRecorder.stop();
}

void VOIP::Send() {
	Record();
	BroadcastVoice();
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