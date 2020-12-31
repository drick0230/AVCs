// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_client.h"
#define tcpActive false

std::string GetNextCommand();

namespace Main {
	const unsigned short serverPort = 11111;
	//std::string myIP = "24.212.42.80";
	//std::string myLocalIP = "192.168.1.141";
	unsigned long myLocalIP = INADDR_ANY;
	unsigned int msBetweenKeepAlive = 10;

	DevicesManager devManager;
}

int main()
{
	////////// AUDIO TEST ////////
	Main::devManager.EnumerateDevices();
	std::wcout << Main::devManager.GetDevicesName(DevicesTypes::AUD_CAPT, 0) << '\n';
	std::wcout << Main::devManager.GetDevicesName(DevicesTypes::AUD_REND, 0) << '\n';
	std::wcout << Main::devManager.GetDevicesName(DevicesTypes::VID_CAPT, 0) << '\n';
	Main::devManager.sr_sw.SetActiveDevice(Main::devManager.audioCaptureDevices[0]);
	Main::devManager.sr_sw.SetActiveDevice(Main::devManager.audioRenderDevices[0]);

	//////// MEMORY LEAK TEST//////
	//Main::devManager.sr_sw.SetInputMediaType(Main::devManager.sr_sw.GetAudioCaptureDeviceMediaTypeDatas()); //Set the Media Type
	//while (1) {
	//	long long _audioDatasTime;
	//	std::vector<unsigned char> _audioDatas = Main::devManager.sr_sw.ReadAudioDatas(_audioDatasTime);
	//	Main::devManager.sr_sw.PlayAudioDatas(_audioDatas, _audioDatasTime);
	//}

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
	} while (userInput == "0" && userInput == "1");

	if (userInput == "0") {
		// Is Client
		Network::udp[0].Bind(Main::myLocalIP, 0);

		std::thread tclientUDP(&clientUDP);
		tclientUDP.detach();
	}
	else {
		// Is Server
		Network::udp[0].Bind(Main::myLocalIP, Main::serverPort);

		Console::Write("Serveur ouvert sur le port "); Console::Write(Main::serverPort); Console::Write('\n');
		Console::Write();

		std::thread tserverUDP(&serverUDP);
		tserverUDP.detach();
	}

	while (1) std::this_thread::sleep_for(std::chrono::milliseconds(1000));


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
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Reduce CPU Usage
	}

	Console::Write('\n');
	Console::Write();

	return consoleIn;
}

void serverUDP() {
	std::string _localIP;
	std::string _publicIP;

	std::string _str;
	unsigned short _uShort;

	// Get Local and Public IP
	Console::Write("IP local du Serveur:");
	Console::Write();
	Console::Write("192.168.1.1\n");
	Console::Write();
	_localIP = "192.168.1.1";// GetNextCommand();

	Console::Write("IP publique du Serveur:");
	Console::Write();
	Console::Write("127.0.0.1\n");
	Console::Write();
	_publicIP = "127.0.0.1";//GetNextCommand();

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

void clientUDP() {
	std::string _serverIP;

	std::string _str;
	unsigned short _uShort;

	// Get Server IP
	Console::Write("IP du Serveur:");
	Console::Write();
	Console::Write("24.212.42.80\n");
	Console::Write();
	_serverIP = "24.212.42.80";//GetNextCommand();


	// Connect to Server
	{
		Packet _packet;

		_packet << std::string("CONNECT_DEMAND");

		Network::udp[0].AddToBook(_serverIP, Main::serverPort);
		Network::udp[0].Send(0, _packet);

		std::thread tKeepAlive(&KeepAlive, 0, 5000);
		tKeepAlive.detach();
	}

	// Establish Connection with other client
	{
		Packet _packet;

		Network::udp[0].WaitReceive(_packet);
		_packet >> _str;
		_packet >> _uShort;

		Network::udp[0].AddToBook(_str, _uShort);

		std::thread tSendAudioNetwork(&SendAudioNetwork, 1);
		tSendAudioNetwork.detach();

		Console::Write("Client : "); Console::Write(_str); Console::Write(':'); Console::Write(_uShort); Console::Write('\n');
		Console::Write();
	}

	std::vector<unsigned char> _audioDatas;
	long long _audioDataTime = -1;

	bool _receivedMediaType = false;
	// Write received Packet
	while (1) {
		unsigned int _clientID;
		Packet _packet;
		_clientID = Network::udp[0].WaitReceive(_packet);

		//Read the type of datas
		if (_packet.Peek("MEDIATYPE")) {
			if (!_receivedMediaType) {
				std::string _str;
				std::vector<unsigned char> _mediaTypeDatas;

				_packet >> _str; // Get "MEDIATYPE"
				_packet >> _mediaTypeDatas; // Get the Media Type Datas

				Main::devManager.sr_sw.SetInputMediaType(_mediaTypeDatas); //Set the Media Type

				_receivedMediaType = true;
			}
		}
		else if (_receivedMediaType) {
			if (_packet.Peek("AUDIO_DATAS")) {
				// We are at the beginning of a new AUDIO_DATAS
				//Play the actual datas
				if (_audioDatas.size() > 0 && _audioDataTime != -1) {
					Main::devManager.sr_sw.PlayAudioDatas(_audioDatas, _audioDataTime);
				}
				_audioDatas.clear();

				std::string _str;
				_packet >> _str; // Get "AUDIO_DATAS"
				_packet >> _audioDataTime; // Get Audio Data Time

				Console::Write("New AUDIO_DATAS\n");
				Console::Write();
			}
			else {
				// Reading the actual Audio Datas
				for (unsigned int _i = 0; _i < _packet.size(); _i++)
					_audioDatas.push_back(_packet.data()[_i]);

				Console::Write("Get AUDIO_DATAS\n");
				Console::Write();
			}
		}
	}
}

void SendAudioNetwork(unsigned int _clientID) {
	while (1) {
		// Send the Mediatype each 10 Audio datas (in case the other client didnt receive it)
		{
			Packet _packet;

			_packet << std::string("MEDIATYPE"); // Write the type of datas
			_packet << Main::devManager.sr_sw.GetAudioCaptureDeviceMediaTypeDatas(); // Write the Media Type datas

			Network::udp[0].Send(_clientID, _packet);
		}
		
		// Send 10 Audio Datas
		for(unsigned int _iAudioDatas = 0; _iAudioDatas < 10; _iAudioDatas++) {
			long long _audioDatasTime;
			std::vector<unsigned char> _audioDatas = Main::devManager.sr_sw.ReadAudioDatas(_audioDatasTime);

			unsigned int _nbPackets = _audioDatas.size() / 512 + 1;
			unsigned int _actualData = 0;

			// New Audio Datas Packet
			{
				Packet _packet;

				_packet << std::string("AUDIO_DATAS"); // Write Start new Audio Datas
				_packet << _audioDatasTime;	 // Write the Time

				Network::udp[0].Send(_clientID, _packet); // Send the Packet
			}

			// Send the Audio Datas in multiple Packets of 512 Bytes
			for (unsigned _iPackets = 0; _iPackets < _nbPackets && _actualData < _audioDatas.size(); _iPackets++) {
				Packet _packet;
				// Write the datas
				for (unsigned int _i2 = 0; _i2 < 512 && _actualData < _audioDatas.size(); _i2++) {
					_packet << _audioDatas[_actualData];
					_actualData++;
				}
				Network::udp[0].Send(_clientID, _packet);
			}
		}
	}
}

void KeepAlive(unsigned int _clientID, unsigned int _ms) {
	while (1) {
		Packet _packet;
		_packet << std::string("KEEP_ALIVE");
		Network::udp[0].Send(_clientID, _packet);
		std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
	}
}