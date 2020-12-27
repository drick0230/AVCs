// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_client.h"
#define tcpActive false

std::string GetNextCommand();

namespace Main {
	std::string myIP = "24.212.42.80";
	//std::string myLocalIP = "192.168.1.141";
	unsigned long myLocalIP = INADDR_ANY;

	unsigned short myPort;
}

int main()
{
	Network::Initialize();
	Network::Add(ProtocoleTypes::UDP, 2);
	Console::InitializeConsole();

	Console::Write("\nVotre port :\n");
	Console::Write();
	Main::myPort = myParse<unsigned short>(GetNextCommand());

	Network::udp[0].Bind(Main::myLocalIP, Main::myPort);
	std::thread tserverUDP(&serverUDP);
	tserverUDP.detach();

	std::thread tclientUDP(&clientUDP);
	tclientUDP.detach();

	while (GetNextCommand() != "quit");


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

	return consoleIn;
}

void serverUDP() {
	Packet _packet;
	Packet _packet2;

	std::string _str;

	Network::udp[0].WaitReceive(_packet);
	_packet.move(0);
	_packet >> _str;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	_packet2.move(0);
	_packet2 << std::string("test1");
	Network::udp[0].Send(0, _packet2);

	Console::Write(_str);
	Console::Write();
}

void clientUDP() {
	Packet _packet;
	std::string _str;

	Network::udp[1].Bind(Main::myLocalIP, Main::myPort + 1);

	std::this_thread::sleep_for(std::chrono::milliseconds(4000));

	Network::udp[1].Connect(Main::myIP, Main::myPort);

	Network::udp[1].WaitReceive(_packet);
	_packet.move(0);
	_packet >> _str;

	Console::Write(_str);
	Console::Write();
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