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
	unsigned int msBetweenKeepAlive = 0;
}

int main()
{
	////////// AUDIO TEST ////////
	DevicesManager devManager;
	devManager.EnumerateDevices();
	std::wcout << devManager.GetDevicesName(DevicesTypes::AUD_CAPT, 0) << '\n';
	std::wcout << devManager.GetDevicesName(DevicesTypes::AUD_REND, 0) << '\n';
	std::wcout << devManager.GetDevicesName(DevicesTypes::VID_CAPT, 0) << '\n';
	devManager.mediaSession.SetActiveDevice(devManager.audioCaptureDevices[0]);
	devManager.mediaSession.SetActiveDevice(devManager.audioRenderDevices[0]);
	devManager.mediaSession.PlayAudioCaptureDatas();
	while (1) Sleep(1000);


	////////// PROGRAM ///////////
	Network::Initialize();
	Network::Add(ProtocoleTypes::UDP, 1);
	Console::InitializeConsole();

	Console::Write("Delais entre les Keep Alive:");
	Console::Write();
	Main::msBetweenKeepAlive = myParse<unsigned int>(GetNextCommand());

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

	while (1);


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
	_localIP = GetNextCommand();

	Console::Write("IP publique du Serveur:");
	Console::Write();
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

void clientUDP() {
	std::string _serverIP;

	std::string _str;
	unsigned short _uShort;

	// Get Server IP
	Console::Write("IP du Serveur:");
	Console::Write();
	_serverIP = GetNextCommand();


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
		//Network::udp[0].AddToBook("192.168.1.141", _uShort);

		Packet _packet2;
		_packet2 << std::string("HOLE_PUNCHING");
		//Network::udp[0].Send(2, _packet2);

		std::thread tKeepAlive(&KeepAlive, 1, Main::msBetweenKeepAlive);
		tKeepAlive.detach();

		Console::Write("Client : "); Console::Write(_str); Console::Write(':'); Console::Write(_uShort); Console::Write('\n');
		Console::Write();
	}

	// Write received Packet
	while (1) {
		unsigned int _clientID;
		Packet _packet;
		_clientID = Network::udp[0].WaitReceive(_packet);

		_packet >> _str;

		Console::Write(_str);
		Console::Write('\n');
		Console::Write();
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

//
//void serverTCP() {
//	Network::tcp[0].WaitClientConnection();
//	Network::tcp[0].WaitReceive(0);
//	std::cout << "Someone is connected\n";
//	Network::tcp[0].Send(0, "Test Message 2!");
//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//}
//
//void clientTCP() {
//	if (Network::tcp[1].Connect("127.0.0.1", 22))
//	{
//		std::cout << "U are connected!\n";
//		Network::tcp[1].Send("Test Message!");
//		Network::tcp[1].WaitReceive();
//	}
//	else
//		throw "Hosting problem";
//}
//
//void serverUDP() {
//	//Network::udp.WaitClientConnection();
//	Network::udp[0].WaitReceive(0);
//	std::cout << "Someone is connected\n";
//	Network::udp[0].WaitReceive(0);
//	Network::udp[0].Send(0, "Test Message 2!");
//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//}
//
//void clientUDP() {
//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//	if (Network::udp[1].Connect("127.0.0.1", 22))
//	{
//		std::cout << "U are connected!\n";
//		Network::udp[1].Send(0, "Test Message!");
//		Network::udp[1].WaitReceive();
//	}
//	else
//		throw "Hosting problem";
//}

/*
void ConsoleIO(bool* isConnected, bool* voiceEnable, bool* speakerEnable, bool* isRecording, VOIP* voip) {
	// Command-Line
	std::vector<std::string> splitCommand;

	// main Loop
	while (true)
	{
		if (Console::Read())
		{
			while (Console::inKeys.size() > 0) {
				unsigned short _inKey = Console::GetInKeys(0);
				_inKey = 0;
				/*if (_inKey == '\r') { // Touche Enter
					// Split le texte entrant en commandes séparées par des espaces
					splitCommand.clear();
					for (unsigned int i = 0; i < Console::inText.content.size(); i++) {
						splitCommand.emplace_back();

						while (Console::inText.content[i] != ' ' && Console::inText.content[i] != '\0') {
							splitCommand.back().push_back(Console::inText.content[i]);
							i++;
						}
					}
					Console::GoTo(1, 23);
					Console::EraseChar(Console::inText.content.size());
					Console::inText.content = "";

					// Faire l'action correspondant aux commandes entrées
					if (splitCommand[0] == "connect") {
						voip->Connect(splitCommand[1], myParse<unsigned int>(splitCommand[2]));
						*isConnected = true;
					}
					else if (splitCommand[0] == "send") {
						voip->Send();
					}
					else if (splitCommand[0] == "receive") {
						voip->Receive();
						voip->TreatAudio();
					}
					else if (splitCommand[0] == "enable") {
						if (splitCommand[1] == "voice")
							*voiceEnable = true;
						else if (splitCommand[1] == "speaker")
							*speakerEnable = true;
					}
					else if (splitCommand[0] == "disable") {
						if (splitCommand[1] == "voice")
							*voiceEnable = false;
						else if (splitCommand[1] == "speaker")
							*speakerEnable = false;
					}
				}
				else if (_inKey == (char)127 && Console::inText.content.size() > 0) { // Touche Effacer
					// Effacer le dernier caractère du texte
					Console::inText.content.pop_back();
				}*//*
			}
			//Console::inText.Show();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}*/