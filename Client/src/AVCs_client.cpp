// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_client.h"
#define tcpActive false

int main()
{
	// Media Session
	PROPVARIANT _propVar;
	PropVariantInit(&_propVar);
	_propVar.vt = VT_EMPTY;

	// Network
	std::string ipAdress;
	bool isConnected = false;
	bool voiceEnable = false;
	bool speakerEnable = false;
	bool isRecording = false;

	// Console
	Console::InitializeConsole();

	// Obtention de l'IP et du Port
	//std::cout << "Votre adresse IP : \n";
	//std::cin >> ipAdress;
	ipAdress = "localhost";
	// Network Test
	Network::Initialize();
	Network::Add(ProtocoleTypes::BOTH);
	Network::Add(ProtocoleTypes::UDP);

	// TCP Test
	if (tcpActive) {
		if (!Network::tcp[0].Host(22))
			throw "Hosting problem";

		std::thread serverThread(serverTCP);
		std::thread clientThread(clientTCP);
		serverThread.detach();
		clientThread.detach();
	}
	// UDP test
	else {
		if (!Network::udp[0].Bind("127.0.0.1", 22))
			throw "Hosting problem";
		if (!Network::udp[1].Bind("127.0.0.1", 23))
			throw "Hosting problem";

		std::thread serverThread(serverUDP);
		std::thread clientThread(clientUDP);
		serverThread.detach();
		clientThread.detach();
	}


	while (1) Sleep(1000);

	// Device Manager Test
	DevicesManager devManager;
	devManager.EnumerateDevices();
	std::wcout << devManager.GetDevicesName(DevicesTypes::AUD_CAPT, 0) << '\n';
	std::wcout << devManager.GetDevicesName(DevicesTypes::AUD_REND, 0) << '\n';
	std::wcout << devManager.GetDevicesName(DevicesTypes::VID_CAPT, 0) << '\n';

	//// MediaSession example
	//devManager.mediaSession.SetActiveDevice(devManager.audioCaptureDevices[0]);
	//devManager.mediaSession.SetActiveDevice(devManager.audioRenderDevices[0]);
	//devManager.mediaSession.PlayAudioCaptureDatas();

	// SourceReader_SinkWritter example
	devManager.sr_sw.SetActiveDevice(devManager.audioCaptureDevices[0]);
	devManager.sr_sw.SetActiveDevice(devManager.audioRenderDevices[0]);
	devManager.sr_sw.PlayAudioCaptureDatas();
	while (1) Sleep(1000);

	// Peak sur le processeur [!]
	//VOIP voip(ipAdress, sf::Socket::AnyPort); // Binding sur le port en UDP
	//std::cout << "Votre Port est <" << voip.socket.getLocalPort() << ">\n";
	// Fin peak sur le processeur

	ATH::ATHElement mainMenu(Vector2_int(1, 1));
	ATH::Rect testrect(&mainMenu, Vector2_int(0, 0), Vector2_int(9, 6), Color(125, 125, 125));
	ATH::SimpleText testText(&testrect, "bonjour\ncomment\nca va", Vector2_int(1, 1), Vector2_int(7, 4), Color(180, 180, 0), Color(125, 125, 125));

	mainMenu.Show();
	//mainMenu.Hide();
	/*
	while (true) {
		ConsoleIO(&isConnected, &voiceEnable, &speakerEnable, &isRecording, &voip);

		if (isConnected) {
			if (voiceEnable)
			{
				voip.Send();
			}

			if (speakerEnable) {
				voip.Receive();
				voip.TreatAudio();
				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}
			//voip.Update();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}*/

	PropVariantClear(&_propVar);

	return 0;
}


void serverTCP() {
	Network::tcp[0].WaitClientConnection();
	Network::tcp[0].WaitReceive(0);
	std::cout << "Someone is connected\n";
	Network::tcp[0].Send(0, "Test Message 2!");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void clientTCP() {
	if (Network::tcp[0].Connect("127.0.0.1", 22))
	{
		std::cout << "U are connected!\n";
		Network::tcp[0].Send("Test Message!");
		Network::tcp[0].WaitReceive();
	}
	else
		throw "Hosting problem";
}

void serverUDP() {
	//Network::udp.WaitClientConnection();
	Network::udp[0].WaitReceive(0);
	std::cout << "Someone is connected\n";
	Network::udp[0].WaitReceive(0);
	Network::udp[0].Send(0, "Test Message 2!");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void clientUDP() {
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	if (Network::udp[1].Connect("127.0.0.1", 22))
	{
		std::cout << "U are connected!\n";
		Network::udp[1].Send(0, "Test Message!");
		Network::udp[1].WaitReceive();
	}
	else
		throw "Hosting problem";
}

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