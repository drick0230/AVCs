#pragma once
#include <string>
#include <vector>
#include <SFML/Network.hpp>
#include<SFML/System/Time.hpp>
#include <iostream>
#include <thread>			// std::this_thread::sleep_for
#include <mutex>
#include <chrono>			// std::chrono::seconds
#include <room.h>
#include "general.h"

class Client
{
protected:
	sf::TcpSocket socket;
public:
	Client(sf::IpAddress ip, unsigned short port);
	~Client();

	void ConnectRoom();
};