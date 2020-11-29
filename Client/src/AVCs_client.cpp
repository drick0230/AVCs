// AVCs_5.cpp : définit le point d'entrée de l'application.
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

	// Console
	Console console(83, 24, 1, 23, 80, 1, 1, 1, 80, 20);


	// Obtention de l'IP et du Port
	std::cout << "Votre adresse IP : \n";
	std::cin >> ipAdress;

	VOIP voip(ipAdress, sf::Socket::AnyPort); // Binding sur le port en UDP
	std::cout << "Votre Port est <" << voip.socket.getLocalPort() << ">\n";

	// MultiThreading
	std::thread threadConsoleIO();

	MainMenu(console);

	while (true) {
		ConsoleIO(console, &isConnected, &voiceEnable, &speakerEnable, &isRecording, &voip);

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

void ConsoleIO(Console &console, bool* isConnected, bool* voiceEnable, bool* speakerEnable, bool* isRecording, VOIP* voip) {
	// Command-Line
	std::vector<std::string> splitCommand;

	// main Loop
	while (true)
	{
		if (console.Read())
		{
			while (console.inKeys.size() > 0) {
				char _inKey = console.GetInKeys(0);
				if (_inKey == '\r') { // Touche Enter
					// Split le texte entrant en commandes séparées par des espaces
					splitCommand.clear();
					for (unsigned int i = 0; i < console.inText.content.size(); i++) {
						splitCommand.emplace_back();

						while (console.inText.content[i] != ' ' && console.inText.content[i] != '\0') {
							splitCommand.back().push_back(console.inText.content[i]);
							i++;
						}
					}
					console.GoTo(1, 23);
					console.EraseChar(console.inText.content.size());
					console.inText.content = "";

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
				else if (_inKey == (char)127 && console.inText.content.size() > 0) { // Touche Effacer
					// Effacer le dernier caractère du texte
					console.inText.content.pop_back();

					// Effacer le dernier caractère dans la console
					console.GoTo(1, 23);
					console.Move(console.inText.content.size(), 0);
					console.EraseChar();
				}
			}

			console.GoTo(1, 23);
			std::cout << console.inText.content;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

template <class T>
T myParse(std::string _string) {
	T _return = 0;

	if (_string[0] >= '0' && _string[0] <= '9') {
		_return = _string[0] - '0';

		unsigned i = 1;
		while (_string[i] >= '0' && _string[i] <= '9') {
			_return *= 10;
			_return += _string[i] - '0';
			i++;
		}
	}
	else {
		std::cout << "Error : < T myParse(std::string _string) > invalid string _string";
		return -1;
	}

	return _return;
}


void MainMenu(Console& _console) {
	_console.GoTo(0, 0);
	_console.SetFontColor(255, 255, 255);
	_console.SetScreenColor(20, 20, 20);

	// Top Line
	for (unsigned char _i = 0; _i < 83; _i++)
		printf_s("%c", (char)219);

	std::string _commands = "";
	// Side Border
	for (int _i = 1; _i < 25; _i++) {
		_console.GoTo(0, _i);
		printf_s("%c", (char)219);
		_console.EraseChar(82);
		_console.GoTo(81, _i);
		printf_s("%c", (char)219);
	}
	//std::cout << _commands;

	// Mid Line
	_console.GoTo(0, 22);
	for (unsigned char _i = 0; _i < 82; _i++)
		printf_s("%c", (char)219);

	// Bot Line
	_console.GoTo(0, 24);
	for (unsigned char _i = 0; _i < 82; _i++)
		printf_s("%c", (char)219);
}