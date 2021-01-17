// AVCs_5.h : fichier Include pour les fichiers Include système standard,
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
	unsigned int networkID;

	AudioDatas audioDatas;

	std::mutex audioBufferMutex;
	std::queue<AudioDatas> audioDatasBuffer;

	bool receivedMediaType;

	std::mutex audioIsProcessing;

	VOIPClient(unsigned int _networkID, bool _receivedMediaType = false) : networkID(_networkID), audioDatas(), receivedMediaType(_receivedMediaType) {};
	VOIPClient(const VOIPClient& _b) : VOIPClient(_b.networkID, _b.receivedMediaType) {};
};

std::string GetNextCommand(); // Get the next line ending by Enter in the console and write the character during typing

// Treat the received Packet
void NetServerReceive(std::string _defaultGateway, std::string _publicIP, std::mutex* _programIsExiting); // Server side
void NetClientReceive(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex, std::mutex* _programIsExiting); // Client side

void NetClientSend(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex, std::mutex* _programIsExiting); // Start sending to other clients

// Send Specific Datas to VOIPClients
void NetSendAudioDatas(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex); // Send the Audio Datas 
void NetSendMediaType(std::vector<VOIPClient>& _clients, std::mutex* _clientsMutex); // Send the Mediatype

void ProcessAudioDatas(std::vector<VOIPClient>& _clients, unsigned int _clientID, std::mutex* _clientsMutex);

void KeepAlive(unsigned int _clientID, unsigned int _ms, std::mutex* _programIsExiting); // Send a Packet to maintain a connection (Hole Punching)

 // Get the VOIPClient's ID by the network ID or return the size of VOIPClient's vector
size_t GetVOIPClient(std::vector<VOIPClient> _clients, unsigned int _networkID);

