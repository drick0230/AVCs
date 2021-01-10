﻿// AVCs_5.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets.

#pragma once

//#include <SFML/Audio.hpp>
#include <thread>			// std::this_thread::sleep_for
#include <chrono>			// std::chrono::seconds
#include <queue>			// std::queue
#include <mutex>			// Protected varaible for multithreading

#include "ATHElement.h"
#include "Audio.h"
#include "Network.h"
#include "general.h"

class VOIPClient {
public:
	VOIPClient(unsigned int _networkID) : networkID(_networkID), audioDatasDuration(0), audioDatasTime(0), receivedMediaType(false) {};
	VOIPClient(const VOIPClient& _b) : networkID(_b.networkID) {};
	unsigned int networkID;

	std::vector<unsigned char> audioDatas;
	long long audioDatasDuration = 0;
	long long audioDatasTime = 0;

	std::mutex audioBufferMutex;
	std::queue<std::vector<unsigned char>> audioDatasBuffer;
	std::queue<long long> audioDatasTimeBuffer;
	std::queue<long long> audioDatasDurationBuffer;

	bool receivedMediaType = false;

	std::mutex audioIsProcessing;
};

//void ConsoleIO(bool*, bool*, bool*, bool*, VOIP*);


void serverTCP();
void clientTCP();

void serverUDP();
//void clientUDP();

void NetSendAudioDatas(unsigned int _clientID);
void NetReceive();

void ProcessAudioDatas(VOIPClient* _client);


// Send a Packet to be able to receive Packet from distant connection
void KeepAlive(unsigned int _clientID, unsigned int _ms);

// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.


