#include "client.h"

Client::Client(sf::IpAddress ip, unsigned short port)
{
	if (socket.connect(ip, port, sf::milliseconds(5000)) == sf::Socket::Status::Error)std::cout << "error" << endl;
}

Client::~Client()
{
	socket.disconnect();
}