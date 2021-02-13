// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_client.h"

// Global constant
namespace Main {
	const unsigned short serverPort = 11111;

	unsigned int msBetweenKeepAlive = 10;

	//DevicesManager devManager;

	const unsigned int minAudioBuffer = 0;

	size_t _audioRenderID, _audioCaptureID;
}

int main()
{
	static std::mutex clientsMutex;
	std::vector<VOIPClient> clients;

	static std::mutex programIsExiting;
	programIsExiting.lock();

	////////// AUDIO TEST ////////
	DevicesManager::Initialize();

	DevicesManager::EnumerateDevices(DevicesTypes::AUD_CAPT);
	DevicesManager::EnumerateDevices(DevicesTypes::AUD_REND);
	//std::wcout << Main::devManager.GetDevicesName(DevicesTypes::VID_CAPT, 0) << '\n';

	//DevicesManager::audioRenderDevices[0].AddTrack();

	////////// PROGRAM ///////////
	Network::Initialize();
	Network::Add(ProtocoleTypes::UDP, 1);
	Console::InitializeConsole();

	std::string userInput = "";
	do {
		Console::Write("[0] Client\n[1] Server\n");
		Console::Write();

		userInput = GetNextCommand();
	} while (userInput != "0" && userInput != "1");

	if (userInput == "0") {
		// Is Client
		std::string _serverIP;

		// Select Mic
		do {
			Console::Write("Select your Microphone :\n");
			for (int _i = 0; _i < DevicesManager::audioCaptureDevices.size(); _i++) {
				Console::Write('[');
				Console::Write(_i);
				Console::Write("] : ");
				Console::Write(DevicesManager::audioCaptureDevices[_i].GetName());
				Console::Write('\n');
			}
			Console::Write('\n');
			Console::Write();

			userInput = GetNextCommand();
		} while (myParse<size_t>(userInput) < 0 || myParse<size_t>(userInput) >= DevicesManager::audioCaptureDevices.size());

		Main::_audioCaptureID = myParse<size_t>(userInput);

		// Select Speaker
		do {
			Console::Write("Select your Speaker/Headphone :\n");
			for (int _i = 0; _i < DevicesManager::audioRenderDevices.size(); _i++) {
				Console::Write('[');
				Console::Write(_i);
				Console::Write("] : ");
				Console::Write(DevicesManager::audioRenderDevices[_i].GetName());
				Console::Write('\n');
			}
			Console::Write('\n');
			Console::Write();

			userInput = GetNextCommand();
		} while (myParse<size_t>(userInput) < 0 || myParse<size_t>(userInput) >= DevicesManager::audioRenderDevices.size());

		Main::_audioRenderID = myParse<size_t>(userInput);


		Network::udp[0].Bind(INADDR_ANY, 0);

		// Connect to Server
		Console::Write("IP du Serveur:");
		Console::Write();
		_serverIP = GetNextCommand();

		if (_serverIP == "0") _serverIP = "127.0.0.1"; // Localhost 

		Network::udp[0].AddToBook(_serverIP, Main::serverPort);

		Network::udp[0].BeginReceiving();
		std::thread tKeepAlive(&KeepAlive, (unsigned int)0, (unsigned int)5000, &programIsExiting); // Start a thread to maintain connection with the server
		std::thread tNetClientReceive(&NetClientReceive, std::ref(clients), &clientsMutex, &programIsExiting); // Start a thread to receive and treat Packets
		tNetClientReceive.detach();

		std::thread tNetClientSend(&NetClientSend, std::ref(clients), &clientsMutex, &programIsExiting); // Start a thread to send Packets to other clients

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

		programIsExiting.unlock(); // Exit the other threads
		tKeepAlive.join(); // Wait tKeepAlive finish current tasks
		tNetClientSend.join();
		//tNetClientReceive.join(); // Wait tNetClientReceive finish current tasks
	}
	else {
		// Is Server
		std::string _defaultGateway;
		std::string _publicIP;

		Console::Write("Passerelle par défaut:");
		Console::Write();
		_defaultGateway = GetNextCommand();


		Console::Write("IP publique du Serveur:");
		Console::Write();
		_publicIP = GetNextCommand();

		Network::udp[0].Bind(INADDR_ANY, Main::serverPort);

		Console::Write("Serveur ouvert sur le port "); Console::Write(Main::serverPort); Console::Write('\n');
		Console::Write();

		Network::udp[0].BeginReceiving();
		std::thread tNetServerReceive(&NetServerReceive, _defaultGateway, _publicIP, &programIsExiting); // Start a thread to get and treat Packets
		tNetServerReceive.detach();

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

		programIsExiting.unlock(); // Exit the other threads
		//tNetServerReceive.join(); // Wait tNetServerReceive finish current tasks
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
				else if (Console::inKeys[_i] != KEYS::SHIFT) {
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

void NetServerReceive(std::string _defaultGateway, std::string _publicIP, std::mutex* _programIsExiting) {
	unsigned char _rcvPacketMsgType; // Received packet's message type

	// Wait the connection of a client
	while (!_programIsExiting->try_lock()) {
		RcvNetPacket* _rcvPacket; // Received packet

		if ((_rcvPacket = Network::udp[0].GetNetPacket()) != NULL) {
			*_rcvPacket >> _rcvPacketMsgType; // MessageType 

			// Wait a bit to be sure the client are ready to receive packets
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));

			// Send ip and port of each clients to other clients
			for (unsigned int _from = 0; _from < Network::udp[0].addressBook.size(); _from++) {
				SendNetPacket _packet;
				_packet << unsigned char(0); // Msg Type
				_packet << (Network::udp[0].addressBook[_from] == _defaultGateway ? _publicIP : Network::udp[0].addressBook[_from]); // Public IP of the client
				_packet << Network::udp[0].portBook[_from]; // Port of the client

				for (unsigned int _to = 0; _to < Network::udp[0].addressBook.size(); _to++) {
					if (_from != _to) {
						Network::udp[0].Send(_to, _packet);
					}
				}
			}

			Console::Write((int)_rcvPacket->clientID);
			Console::Write(":");
			Console::Write(_rcvPacketMsgType);
			Console::Write('\n');
			Console::Write();

			delete _rcvPacket;
		}
		else { std::this_thread::sleep_for(std::chrono::milliseconds(20)); /* Reduce CPU Usage */ }
	}

	_programIsExiting->unlock();
}
void NetClientReceive(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex, std::mutex* _programIsExiting) {
	unsigned char _rcvPacketMsgType; // Received packet's message type

	while (!_programIsExiting->try_lock()) {
		RcvNetPacket* _rcvPacket; // Received packet

		if ((_rcvPacket = Network::udp[0].GetNetPacket()) != NULL) {
			*_rcvPacket >> _rcvPacketMsgType; // MessageType 

			if (_rcvPacket->clientID == 0) { // Packet from the server
				std::string _ipAddress; // Received IP Address in case 0
				unsigned short _port; // Received Port in case 0
				unsigned int _networkID; // Returned Network Client ID in case 0
				unsigned int _clientID; // Returned Client ID in case 0

				std::unique_lock<std::mutex> LG_udp(Network::udp[0].inUse, std::defer_lock); // Lock udp[0] in case 0

				// Treat the message type
				switch (_rcvPacketMsgType)
				{
				case 0: // Client's connection  information
					*_rcvPacket >> _ipAddress;
					*_rcvPacket >> _port;

					LG_udp.lock();
					if ((_networkID = Network::udp[0].PosInBook(_ipAddress, _port)) == Network::udp[0].addressBook.size()) // Not in the book
						_networkID = Network::udp[0].AddToBook(_ipAddress, _port);
					LG_udp.unlock();

					_clientsMutex->lock();
					if ((_clientID = GetVOIPClient(_clients, _networkID)) == _clients.size()) { // Not added as client
						_clients.emplace_back(_networkID); // Add new client
						DevicesManager::audioRenderDevices[Main::_audioRenderID].AddTrack(); // Add a track for the client

						Console::Write(_ipAddress + ':');
						Console::Write(_port);
						Console::Write(" added as Client\n");
						Console::Write();
					}
					_clientsMutex->unlock();

					break;
				default:
					Console::Write("Received an invalid Packet from ");
					Console::Write((int)_rcvPacket->clientID);
					Console::Write('\n');
					Console::Write();
					break;
				}
			}
			else { // Packet from a client
				std::lock_guard<std::mutex> _clientsMutexLG(*_clientsMutex); // Lock _clientsMutex and unlock it at destruct
				unsigned int _clientID = GetVOIPClient(_clients, _rcvPacket->clientID); // VOIPClient's ID

				if (_clientID < _clients.size()) {
					// Treat the message type
					switch (_rcvPacketMsgType)
					{
					case 3: // Reading Audio Datas
						// _client->audioDatas.datas.resize(_client->audioDatas.datas.size() + _rcvPacket.size()); // for optimisation LATER
						for (unsigned int _i = 1; _i < _rcvPacket->size(); _i++)
							_clients[_clientID].audioDatas.datas.push_back(_rcvPacket->data()[_i]);
						break;
					case 2: // Begin new Audio Datas
						if (_clients[_clientID].receivedMediaType) {
							*_rcvPacket >> _clients[_clientID].audioDatas;
							DevicesManager::audioRenderDevices[Main::_audioRenderID].Play(_clients[_clientID].audioDatas, _clientID);
							Console::Write("Received new Audio Datas from ");
							Console::Write((int)_rcvPacket->clientID);
							Console::Write('\n');
							Console::Write();
						}
						break;
					case 1: // Media Type
						if (!_clients[_clientID].receivedMediaType) {
							std::vector<unsigned char> _mediaTypeDatas;

							*_rcvPacket >> _mediaTypeDatas; // Get the Media Type Datas

							DevicesManager::audioRenderDevices[Main::_audioRenderID].SetInputMediaType(_mediaTypeDatas, _clientID); //Set the Media Type

							_clients[_clientID].receivedMediaType = true;

							Console::Write("Received media type for ");
							Console::Write((int)_rcvPacket->clientID);
							Console::Write('\n');
							Console::Write();
						}
						break;
					case 0: // Keep Alive
						Console::Write((int)_rcvPacket->clientID);
						Console::Write(" : Keep Alive\n");
						break;
					default:
						Console::Write("Received an invalid Packet from ");
						Console::Write((int)_rcvPacket->clientID);
						Console::Write('\n');
						Console::Write();
						break;
					}
				}
				else {
					Console::Write("Received a Packet from the invalid Client ");
					Console::Write((int)_rcvPacket->clientID);
					Console::Write('\n');
					Console::Write();
				}
			}

			delete _rcvPacket;
		}
		else { std::this_thread::sleep_for(std::chrono::milliseconds(20)); /* Reduce CPU Usage */ }
	}

	_programIsExiting->unlock();
}

// Send the mediatype and 50 audio datas
void NetClientSend(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex, std::mutex* _programIsExiting) {
	while (!_programIsExiting->try_lock()) { // Exit when _programIsExiting is Unlock
		if (_clients.size() > 0) {
			NetSendMediaType(_clients, _clientsMutex);
			NetSendAudioDatas(_clients, _clientsMutex);
		}
		else { std::this_thread::sleep_for(std::chrono::milliseconds(20)); /* Reduce CPU Usage */ }
	}

	_programIsExiting->unlock();
}

void NetSendAudioDatas(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex) {
	std::lock_guard<std::mutex> _clientsMutexLG(*_clientsMutex); // Lock _clientsMutex and unlock it at destruct

	AudioDatas _audioDatas = DevicesManager::audioCaptureDevices[Main::_audioCaptureID].Read();;

	if (_audioDatas.datas.size() > 0) {
		//unsigned int _nbPackets = _audioDatas.datas.size() / 511 + 1; // 511 is the size of a Packet - the msgType
		//unsigned int _actualData = 0;

		SendNetPacket _packet;
		_packet << unsigned char(2); // Write Start new Audio Datas
		_packet << _audioDatas;

		//// New Audio Datas Packet
		//{
		//	Packet _packet;

		//	_packet << unsigned char(2); // Write Start new Audio Datas
		//	_packet << _audioDatas.duration; // Write the duration
		//	_packet << _audioDatas.time;	 // Write the Time

		//	// Send Packet to all clients
		//	for (unsigned int _i = 0; _i < _clients.size(); _i++)
		//		Network::udp[0].Send(_clients[_i].networkID, _packet); // Send the Packet
		//}

		//// Send the Audio Datas in multiple Packets of 512 Bytes
		//for (unsigned _iPackets = 0; _iPackets < _nbPackets && _actualData < _audioDatas.datas.size(); _iPackets++) {
		//	Packet _packet;
		//	_packet << unsigned char(3);

		//	// Write the datas
		//	for (unsigned int _i2 = 0; _i2 < 511 && _actualData < _audioDatas.datas.size(); _i2++) {
		//		_packet << _audioDatas.datas[_actualData];
		//		_actualData++;
		//	}

			// Send Packet to all clients
		for (unsigned int _i = 0; _i < _clients.size(); _i++)
			Network::udp[0].Send(_clients[_i].networkID, _packet);
	}
}
void NetSendMediaType(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex) {
	SendNetPacket _packet;

	_packet << unsigned char(1); // Write the type of datas
	_packet << DevicesManager::audioCaptureDevices[Main::_audioCaptureID].GetMediaTypeDatas(); // Write the Media Type datas

	// Send Packet to all clients
	std::lock_guard<std::mutex> _clientsMutexLG(*_clientsMutex); // Lock _clientsMutex and unlock it at destruct
	for (unsigned int _i = 0; _i < _clients.size(); _i++)
		Network::udp[0].Send(_clients[_i].networkID, _packet);
}

void ProcessAudioDatas(std::vector<VOIPClient>& _clients, unsigned int _clientID, std::mutex* _clientsMutex) {
	std::lock_guard<std::mutex> _clientsMutexLG(*_clientsMutex); // Lock _clientsMutex and unlock it at destruct
	std::lock_guard<std::mutex> _audioIsProcessingLG(_clients[_clientID].audioIsProcessing); // Lock _clients[_clientID].audioIsProcessing and unlock it at destruct

	// Test if we have a good amount of Audio Datas
	if (_clients[_clientID].audioDatasBuffer.size() >= Main::minAudioBuffer) {
		while (!_clients[_clientID].audioDatasBuffer.empty()) {
			_clients[_clientID].audioBufferMutex.lock(); // Keep other threads from using the buffer

			DevicesManager::audioRenderDevices[Main::_audioRenderID].Play(_clients[_clientID].audioDatasBuffer.front(), _clientID);
			_clients[_clientID].audioDatasBuffer.pop(); // Remove the processed element

			_clients[_clientID].audioBufferMutex.unlock(); // Permit other threads to use the buffer
		}
	}
}

void KeepAlive(unsigned int _clientID, unsigned int _ms, std::mutex* _programIsExiting) {
	while (!_programIsExiting->try_lock()) { // Exit when _programIsExiting is Unlock
		SendNetPacket _packet;
		_packet << unsigned char(0);
		Network::udp[0].Send(_clientID, _packet);
		std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
	}

	_programIsExiting->unlock();
}

size_t GetVOIPClient(std::vector<VOIPClient> _clients, unsigned int _networkID) {
	for (size_t _clientID = 0; _clientID < _clients.size(); _clientID++)
		if (_clients[_clientID].networkID == _networkID)
			return _clientID;

	return _clients.size();
}