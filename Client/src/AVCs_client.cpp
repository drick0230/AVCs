﻿// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_client.h"

int main()
{
	// Network
	std::string ipAdress;
	bool isConnected = false;
	bool voiceEnable = false;
	bool speakerEnable = false;
	bool isRecording = false;

	// Obtention de l'IP et du Port
	std::cout << "Votre adresse IP : \n";
	std::cin >> ipAdress;

	VOIP voip(ipAdress, sf::Socket::AnyPort); // Binding sur le port en UDP
	std::cout << "Votre Port est <" << voip.socket.getLocalPort() << ">\n";

	// MultiThreading
	std::thread threadConsoleIO(ConsoleIO, &isConnected, &voiceEnable, &speakerEnable, &isRecording, &voip);
	while (true) {
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
	}


	return 0;
}

void ConsoleIO(bool* isConnected, bool* voiceEnable, bool* speakerEnable, bool* isRecording, VOIP* voip) {

	// Audio Recording
	//sf::SoundBufferRecorder recorder;
	//const sf::SoundBuffer& buffer = recorder.getBuffer();
	//sf::Sound sound;

	// Command-Line
	const unsigned short maxCommandChar = 50;

	char command[maxCommandChar];
	for (unsigned short i = 0; i < maxCommandChar; i++) {
		command[i] = '\0';
	}

	std::vector<std::string> splitCommand;

	// main Loop
	while (true)
	{
		std::cin.getline(command, 30, '\n');
		if (command[0] != '\0') {

			splitCommand = split(command, ' ', true);

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

		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
}