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
	// Command-Line
	Console console;
	const unsigned short maxCommandChar = 50;

	char command[maxCommandChar];
	for (unsigned short i = 0; i < maxCommandChar; i++) {
		command[i] = '\0';
	}

	std::vector<std::string> splitCommand;

	MainMenu(console);
	//std::cout << std::hex << (short)(unsigned char)255;

	// main Loop
	while (true)
	{
		std::cin.getline(command, 30, '\n');
		if (command[0] != '\0') {

			splitCommand.clear();

			unsigned short i = 0;
			while (command[i] != '\0') {
				splitCommand.emplace_back();

				while (command[i] != ' ' && command[i] != '\0') {
					splitCommand.back().push_back(command[i]);
					i++;
				}

				i++;
			}

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
	_console.ConsoleGoTo(0, 0);
	//std::cout << '\x1B' << "[38;2;255;255;255m";
	_console.ConsoleSetFontColor(255, 255, 255);
	//std::cout << '\x1B' << "]4;0;rgb:" << std::hex << (short)0 << "/3d/3d" << '\x1B';
	//_console.ConsoleGoTo(82, 25);
	_console.ConsoleSetScreenColor(30,  30, 30);
	//_console.ConsoleDeleteLine(3);

	// Top Line
	for (unsigned char _i = 0; _i < 83; _i++)
		printf_s("%c", (char)219);

	std::string _commands = "";
	// Side Border
	for (int _i = 1; _i < 25; _i++) {
		_console.ConsoleGoTo(0, _i);
		printf_s("%c", (char)219);
		_console.ConsoleEraseChar(82);
		_console.ConsoleGoTo(81, _i);
		printf_s("%c", (char)219);
	}
	//std::cout << _commands;

	// Mid Line
	_console.ConsoleGoTo(0, 22);
	for (unsigned char _i = 0; _i < 82; _i++)
		printf_s("%c", (char)219);

	// Bot Line
	_console.ConsoleGoTo(0, 24);
	for (unsigned char _i = 0; _i < 82; _i++)
		printf_s("%c", (char)219);
}