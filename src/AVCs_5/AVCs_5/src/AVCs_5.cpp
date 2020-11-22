// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_5.h"

int main()
{
    // Network
    std::string ipAdress;
    unsigned short port;
    bool isConnected = false;

    bool isRecording = false;

    // Audio Recording
    sf::SoundBufferRecorder recorder;
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    sf::Sound sound;

    // Command-Line
    const unsigned short maxCommandChar = 50;

    char command[maxCommandChar];
    for (unsigned short i = 0; i < maxCommandChar; i++) {
        command[i] = '\0';
    }

    std::vector<std::string> splitCommand;

    // Obtention de l'IP et du Port
    std::cout << "Votre adresse IP : \n";
    std::cin >> ipAdress;

    VOIP voip(ipAdress, sf::Socket::AnyPort); // Binding sur le port en UDP
    std::cout << "Votre Port est <" << voip.socket.getLocalPort() << ">\n";

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
                voip.Connect(splitCommand[1], myParse<unsigned int>(splitCommand[2]));
            }
            else if (splitCommand[0] == "send") {
                sf::Packet _packet;
                _packet << "Hello VOIP!!!";
                voip.Broadcast(&_packet);
            }
            else if (splitCommand[0] == "receive") {
                voip.Receive();
                voip.Treat();
            }
            else if (splitCommand[0] == "record" && sf::SoundBufferRecorder::isAvailable() && !isRecording) {
                recorder.start();
                isRecording = true;
            }
            else if (splitCommand[0] == "stop" && sf::SoundBufferRecorder::isAvailable() && isRecording) {
                recorder.stop();
                sound.setBuffer(buffer);
                isRecording = false;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    return 0;
}

void RecordPlaybackAudio() {
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