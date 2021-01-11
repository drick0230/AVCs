// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_client.h"

std::string GetNextCommand();

namespace Main {
	const unsigned short serverPort = 11111;

	unsigned int msBetweenKeepAlive = 10;

	//DevicesManager devManager;

	const unsigned int minAudioBuffer = 50;

	std::vector<VOIPClient> clients;

	SourceReader_SinkWritter sr_sw;

	//std::queue<std::vector<unsigned char>> audioDatasBuffer;
	//std::queue<long long> audioDatasTimeBuffer;
	//std::queue<long long> audioDatasDurationBuffer;

	//std::mutex audioBufferMutex;

	//std::mutex audioIsProcessing;
}

// Client 1 (Track 1)
void client1() {
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	while (!Main::clients[1].audioDatasBuffer.empty()) {

		DevicesManager::audioRenderDevices[0].Play(Main::clients[1].audioDatasBuffer.front(), 1);
		Main::clients[1].audioDatasBuffer.pop();
	}
}

int main()
{
	////////// AUDIO TEST ////////
	DevicesManager::Initialize();

	DevicesManager::EnumerateDevices(DevicesTypes::AUD_CAPT);
	DevicesManager::EnumerateDevices(DevicesTypes::AUD_REND);

	std::wcout << DevicesManager::GetDevicesName(DevicesTypes::AUD_CAPT, 0) << '\n';
	std::wcout << DevicesManager::GetDevicesName(DevicesTypes::AUD_REND, 0) << '\n';
	//std::wcout << Main::devManager.GetDevicesName(DevicesTypes::VID_CAPT, 0) << '\n';

	DevicesManager::audioRenderDevices[0].AddTrack();

	Main::clients.resize(2, VOIPClient(0));
	DevicesManager::audioRenderDevices[0].SetInputMediaType(DevicesManager::audioCaptureDevices[0].GetMediaTypeDatas()); //Set the Media Type for the track 0
	DevicesManager::audioRenderDevices[0].SetInputMediaType(DevicesManager::audioCaptureDevices[0].GetMediaTypeDatas(), 1); //Set the Media Type for the track 1



	while (1) {
		// Client 0 (Track 0)
		for (unsigned int _i = 0; _i < 10; _i++) {
			AudioDatas _audioDatas = DevicesManager::audioCaptureDevices[0].Read();
			Main::clients[0].audioDatasBuffer.emplace(_audioDatas);
			Main::clients[1].audioDatasBuffer.emplace(_audioDatas);
		}

		std::thread tClient1(&client1);

		while (!Main::clients[0].audioDatasBuffer.empty()) {

			DevicesManager::audioRenderDevices[0].Play(Main::clients[0].audioDatasBuffer.front());
			Main::clients[0].audioDatasBuffer.pop();
		}
		tClient1.join();
	}

	////////// PROGRAM ///////////
	Network::Initialize();
	Network::Add(ProtocoleTypes::UDP, 1);
	Console::InitializeConsole();

	//Console::Write("Delais entre les Keep Alive:");
	//Console::Write();
	//Main::msBetweenKeepAlive = myParse<unsigned int>(GetNextCommand());

	std::string userInput = "";
	do {
		Console::Write("[0] Client\n[1] Server\n");
		Console::Write();

		userInput = GetNextCommand();
	} while (userInput != "0" && userInput != "1");

	if (userInput == "0") {
		// Is Client
		std::string _serverIP;

		Network::udp[0].Bind(INADDR_ANY, 0);

		// Connect to Server
		Console::Write("IP du Serveur:");
		Console::Write();
		_serverIP = GetNextCommand();

		Network::udp[0].AddToBook(_serverIP, Main::serverPort);

		std::thread tKeepAlive(&KeepAlive, 0, 5000);
		tKeepAlive.detach();

		std::thread tNetReceive(&NetReceive);
		tNetReceive.detach();

		Console::Write("[0] Exit\n");
		Console::Write();

		while (true) {
			// Exit if press 0 key
			if (Console::Read())
				if (Console::inKeys.size() > 0)
					if (Console::inKeys[0] == '0')
						break;
					else
						Console::inKeys.clear();
			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		}

		tNetReceive.~thread();
		tKeepAlive.~thread();
	}
	else {
		// Is Server
		Network::udp[0].Bind(INADDR_ANY, Main::serverPort);

		Console::Write("Serveur ouvert sur le port "); Console::Write(Main::serverPort); Console::Write('\n');
		Console::Write();

		std::thread tserverUDP(&serverUDP);
		tserverUDP.detach();

		while (true) std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}

	return 0;
}

std::string GetNextCommand() {
	std::string consoleIn;

	bool _quit = false;
	while (!_quit) {
		if (Console::Read())
		{
			for (unsigned int _i = 0; _i < Console::inKeys.size(); _i++) {
				if (Console::inKeys[_i] == '\r')
				{
					_quit = true;
					Console::inKeys.erase(Console::inKeys.begin(), Console::inKeys.begin() + _i);
					break;
				}
				else if (Console::inKeys[_i] == KEYS::DEL) {
					if (consoleIn.length() > 0) {
						Console::Move(-1, 0);
						Console::EraseChar();
						Console::Write();
						consoleIn.pop_back();
					}
				}
				else {
					consoleIn.push_back((char)Console::inKeys[_i]);
					Console::Write((char)Console::inKeys[_i]);
					Console::Write();
				}
			}
			Console::inKeys.clear();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Reduce CPU Usage
	}

	Console::Write('\n');
	Console::Write();

	return consoleIn;
}

void serverUDP() {
	std::string _localIP;
	std::string _publicIP;

	std::string _str;

	// Get Local and Public IP
	Console::Write("Passerelle par défaut:");
	Console::Write();
	//Console::Write("192.168.1.1\n");
	//Console::Write();
	_localIP = GetNextCommand();

	Console::Write("IP publique du Serveur:");
	Console::Write();
	//Console::Write("127.0.0.1\n");
	//Console::Write();
	_publicIP = GetNextCommand();

	// Wait the connection of 2 clients
	while (Network::udp[0].addressBook.size() != 2) {
		Packet _packet;
		Network::udp[0].WaitReceive(_packet);

		_packet >> _str;

		Console::Write(_str);
		Console::Write('\n');
		Console::Write();
	}

	// Wait a bit to be sure the client are ready to receive packets
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// Send ip and port of each other to each other
	for (unsigned int _from = 0; _from < Network::udp[0].addressBook.size(); _from++) {
		for (unsigned int _to = 0; _to < Network::udp[0].addressBook.size(); _to++) {
			if (_from != _to) {
				Packet _packet;
				_packet << (Network::udp[0].addressBook[_from] == _localIP ? _publicIP : Network::udp[0].addressBook[_from]);
				_packet << Network::udp[0].portBook[_from];

				Network::udp[0].Send(_to, _packet);
			}
		}
	}

	// Write received Packet
	while (1) {
		Packet _packet;
		Network::udp[0].WaitReceive(_packet);

		_packet >> _str;

		Console::Write(_str);
		Console::Write('\n');
		Console::Write();
	}
}

// Treat the received Packet
void NetReceive() {
	while (true) {
		unsigned int _clientID;
		unsigned char _msgType;
		Packet _packet;

		_clientID = Network::udp[0].WaitReceive(_packet);

		// Get the VOIPClient by the network ID
		VOIPClient* _client = NULL;
		for (unsigned int _i = 0; _i < Main::clients.size(); _i++) {
			if (Main::clients[_i].networkID == _clientID) {
				_client = &Main::clients[_i];
				break;
			}
		}

		// New client
		if (_client == NULL) {
			Main::clients.emplace_back(_clientID);
			_client = &Main::clients.back();
		}

		_packet >> _msgType; // MessageType 

		switch (_msgType)
		{
		case 3: // Reading Audio Datas
			for (unsigned int _i = 0; _i < _packet.size(); _i++)
				_client->audioDatas.datas.push_back(_packet.data()[_i]);
			break;
		case 2: // Begin new Audio Datas
				//Add the actual Audio Data in the queue
			if (_client->audioDatas.datas.size() > 0 && _client->audioDatas.time != 0) {
				_client->audioBufferMutex.lock();

				_client->audioDatasBuffer.emplace(_client->audioDatas.datas, _client->audioDatas.duration, _client->audioDatas.time);
				//_client->audioDatasDurationBuffer.push(_client->audioDatas.duration);
				//_client->audioDatasTimeBuffer.push(_client->audioDatas.time);

				_client->audioBufferMutex.unlock();

				// If no  Audio Processing thread is running, create a new one
				if (_client->audioIsProcessing.try_lock()) {
					//std::thread tProcessAudioDatas(&ProcessAudioDatas, *_client);
					//tProcessAudioDatas.detach();
					_client->audioIsProcessing.unlock();
				}
			}

			_packet >> _client->audioDatas.duration; // Get Audio Datas Duration
			_packet >> _client->audioDatas.time; // Get Audio Datas Time

			_client->audioDatas.datas.clear();
			break;
		case 1: // Media Type
			if (!_client->receivedMediaType) {
				std::string _str;
				std::vector<unsigned char> _mediaTypeDatas;

				_packet >> _str; // Get "MEDIATYPE"
				_packet >> _mediaTypeDatas; // Get the Media Type Datas

				Main::sr_sw.SetInputMediaType(_mediaTypeDatas); //Set the Media Type

				_client->receivedMediaType = true;
			}
			break;
		case 0: // Keep Alive
			Console::Write((int)_clientID);
			Console::Write(" : Keep Alive\n");
			break;
		default:
			break;
		}
	}
}

// Send the Audio Datas throught network
void NetSendAudioDatas(unsigned int _clientID) {
	AudioDatas _audioDatas = Main::sr_sw.ReadAudioDatas();

	if (_audioDatas.datas.size() > 0) {
		unsigned int _nbPackets = _audioDatas.datas.size() / 512 + 1;
		unsigned int _actualData = 0;

		// New Audio Datas Packet
		{
			Packet _packet;

			_packet << std::string("AUDIO_DATAS"); // Write Start new Audio Datas
			_packet << _audioDatas.duration; // Write the duration
			_packet << _audioDatas.time;	 // Write the Time

			Network::udp[0].Send(_clientID, _packet); // Send the Packet
		}

		// Send the Audio Datas in multiple Packets of 512 Bytes
		for (unsigned _iPackets = 0; _iPackets < _nbPackets && _actualData < _audioDatas.datas.size(); _iPackets++) {
			Packet _packet;
			// Write the datas
			for (unsigned int _i2 = 0; _i2 < 512 && _actualData < _audioDatas.datas.size(); _i2++) {
				_packet << _audioDatas.datas[_actualData];
				_actualData++;
			}
			Network::udp[0].Send(_clientID, _packet);
		}
	}
}

// Send the Mediatype throught network
void NetSendMediaType(unsigned int _clientID) {
	Packet _packet;

	_packet << std::string("MEDIATYPE"); // Write the type of datas
	_packet << Main::sr_sw.GetAudioCaptureDeviceMediaTypeDatas(); // Write the Media Type datas

	Network::udp[0].Send(_clientID, _packet);
}

void ProcessAudioDatas(VOIPClient* _client) {
	std::unique_lock<std::mutex> _uLockMutex(_client->audioIsProcessing); // Unlock the mutex when ProcessAudioDatas() end
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	// Test if we have a good amount of Audio Datas
	if (_client->audioDatasBuffer.size() >= Main::minAudioBuffer) {
		while (!_client->audioDatasBuffer.empty()) {
			_client->audioBufferMutex.lock(); // Keep other threads from using the buffer

			Main::sr_sw.PlayAudioDatas(_client->audioDatasBuffer.front()); //Play the actual datas
			_client->audioDatasBuffer.pop(); // Remove the processed element

			_client->audioBufferMutex.unlock(); // Permit other threads from using the buffer
		}
	}
	_uLockMutex.unlock();
}

void KeepAlive(unsigned int _clientID, unsigned int _ms) {
	while (1) {
		Packet _packet;
		_packet << std::string("KEEP_ALIVE");
		Network::udp[0].Send(_clientID, _packet);
		std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
	}
}