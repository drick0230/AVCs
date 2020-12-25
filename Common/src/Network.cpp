#include "Network.h"

// Static Variables
int Network::hr;

std::vector<TCP> Network::tcp;
std::vector <UDP> Network::udp;

Protocole::Protocole(int _family, int _sockType, int _protocol) : connectSocket(INVALID_SOCKET), hostSocket(INVALID_SOCKET) {
	ZeroMemory(&clientInfo, sizeof(clientInfo));
	clientInfo.ai_family = _family;
	clientInfo.ai_socktype = _sockType;
	clientInfo.ai_protocol = _protocol;

	ZeroMemory(&serverInfo, sizeof(serverInfo));
	serverInfo.ai_family = _family;
	serverInfo.ai_socktype = _sockType;
	serverInfo.ai_protocol = _protocol;
	serverInfo.ai_flags = AI_PASSIVE;
}

Protocole::~Protocole() {}

TCP::TCP() : Protocole(AF_INET, SOCK_STREAM, IPPROTO_TCP) {} // Socket as IPV4 and TCP 
UDP::UDP() : Protocole(AF_INET, SOCK_DGRAM, IPPROTO_UDP), serverAddr(){} // Socket as IPV4 and UDP 

#pragma region Protocole

unsigned int Protocole::WaitReceive(unsigned int _clientID) {
	const unsigned int _recvBufferLength = 512;
	char _recvBuffer[_recvBufferLength];
	int _nbRecvBytes = 0;

	_nbRecvBytes = udpTcpReceive(_clientID, _recvBuffer, _recvBufferLength);

	if (_nbRecvBytes == SOCKET_ERROR) Network::hr = SOCKET_ERROR;
	if (_nbRecvBytes > 0 && Network::hr == 0) {
		// Receive something
		std::cout << "Host receive : " << _nbRecvBytes << " Bytes\n";
		std::cout << "Received Bytes are : \n";

		// Print datas
		unsigned int _i = 0;
		while (_recvBuffer[_i] != '\0' && _i < _nbRecvBytes) {
			std::cout << _recvBuffer[_i];
			_i++;
		}
		std::cout << '\n';
	}

	if (Network::hr != 0) throw Network::hr;
	return _clientID;
}

void Protocole::Send(std::string _str) { Send(-1, _str); }
void Protocole::Send(unsigned int _clientID, std::string _str) { Send(_clientID, (char*)_str.c_str(), _str.size()); }
void Protocole::Send(unsigned int _clientID, char* _bufferToSend, int _bufferToSendLength) {
	int _nbSendBytes = 0;

	_nbSendBytes = udpTcpSend(_clientID, _bufferToSend, _bufferToSendLength);

	if (_nbSendBytes == SOCKET_ERROR) Network::hr = SOCKET_ERROR;
	if (_nbSendBytes != _bufferToSendLength) {
		std::cerr << "Only " << _nbSendBytes << " Bytes was send and not " << _bufferToSendLength << "Bytes\n";
	}

	if (Network::hr != 0) {
		std::cerr << "\n ERROR at void Protocole::Send(unsigned int _clientID, char* _bufferToSend, int _bufferToSendLength) :\n"
				  << WSAGetLastError();
		throw Network::hr;
	}
}
#pragma endregion //Protocole

#pragma region TCP
bool TCP::Host(unsigned short _port, bool _openPort) { return Host(std::to_string(_port), _openPort); }
bool TCP::Host(std::string _port, bool _openPort) {
	struct addrinfo* _result = NULL;

	if (Network::hr == 0) Network::hr = getaddrinfo(NULL, _port.c_str(), &serverInfo, &_result); 	// Resolve the server address and port
	if (Network::hr == 0) hostSocket = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol); // Create a SOCKET for connecting to server
	if (hostSocket == INVALID_SOCKET) {
		Network::hr = INVALID_SOCKET;
		throw hostSocket;
	}

	// Bind the socket
	if (Network::hr == 0) Network::hr = bind(hostSocket, _result->ai_addr, (int)_result->ai_addrlen);
	if (Network::hr == SOCKET_ERROR) {
		std::cerr << "bind failed with error : " << WSAGetLastError() << '\n';
		closesocket(hostSocket);
		hostSocket = INVALID_SOCKET;
	}

	tcpListen(); // Only for TCP

	freeaddrinfo(_result);

	if (Network::hr != 0) {
		throw Network::hr;
		return false;
	}

	return true;
}

bool TCP::Connect(std::string _ipAddress, unsigned short _port, bool _openPort) {
	struct addrinfo* _serverInfo = NULL;

	if (Network::hr == 0) Network::hr = getaddrinfo(_ipAddress.c_str(), std::to_string(_port).c_str(), &clientInfo, &_serverInfo); 	// Resolve the server address and port
	if (Network::hr == 0) connectSocket = socket(_serverInfo->ai_family, _serverInfo->ai_socktype, _serverInfo->ai_protocol); // Create a SOCKET for connecting to server
	if (connectSocket == INVALID_SOCKET) {
		Network::hr = INVALID_SOCKET;
		throw connectSocket;
	}

	// Connect to server
	if (Network::hr == 0) Network::hr = connect(connectSocket, _serverInfo->ai_addr, (int)_serverInfo->ai_addrlen);
	if (Network::hr == SOCKET_ERROR) {
		throw connectSocket;
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}



	if (connectSocket == INVALID_SOCKET) {
		//Unable to connect to server!
		return false;
	}

	freeaddrinfo(_serverInfo);

	if (Network::hr != 0) throw Network::hr;

	return true;
}

void TCP::tcpListen() {
	// Begin listenning on the socket
	if (listen(hostSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error: " << WSAGetLastError() << '\n';
		closesocket(hostSocket);
		throw hostSocket;
	}
}

void TCP::WaitClientConnection() {
	clientsSocket.emplace_back(INVALID_SOCKET);

	// Accept a client socket
	clientsSocket.back() = accept(hostSocket, NULL, NULL);
	if (clientsSocket.back() == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		throw(clientsSocket.back());
		closesocket(hostSocket);
	}

}

int TCP::udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength) {
	SOCKET* _sendToSocket = NULL;

	if (_clientID == -1) _sendToSocket = &connectSocket; // Send to the server
	else _sendToSocket = &clientsSocket[_clientID]; // Send to a client

	return send(*_sendToSocket, _bufferToSend, _bufferToSendLength, 0);
}

int TCP::udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength) {
	SOCKET* recvFromSocket = NULL;

	if (_clientID == -1) recvFromSocket = &connectSocket; // Send to the server
	else recvFromSocket = &clientsSocket[_clientID]; // Send to a client

	return recv(*recvFromSocket, _recvBuffer, _recvBufferLength, 0);
}

#pragma endregion //TCP

#pragma region UDP
bool UDP::Bind(std::string _ipAddress, unsigned short _port) {
	serverAddr = Network::CreateSockaddr_in(AF_INET, _ipAddress, _port);

	hostSocket = socket(serverInfo.ai_family, serverInfo.ai_socktype, serverInfo.ai_protocol);

	// Bind the socket
	if (Network::hr == 0) Network::hr = bind(hostSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (Network::hr == SOCKET_ERROR) {
		std::cerr << "bind failed with error : " << WSAGetLastError() << '\n';
		closesocket(hostSocket);
		hostSocket = INVALID_SOCKET;
	}

	if (Network::hr != 0) {
		throw Network::hr;
		return false;
	}

	return true;
}

bool UDP::Connect(std::string _ipAddress, unsigned short _port, bool _openPort) {
	struct sockaddr_in _sendToAddr = Network::CreateSockaddr_in(AF_INET, _ipAddress, _port);
	int _sendToAddrSize = sizeof(_sendToAddr);
	unsigned long _ipBuffer;
	int _nbSendBytes = 0;

	const std::string _connectionDemandPacket = "CONNECTION_DEMAND";

	//_sendToAddr.sin_family = AF_INET;
	//_sendToAddr.sin_port = htons(_port);
	//inet_pton(AF_INET, _ipAddress.c_str(), &_ipBuffer);
	//_sendToAddr.sin_addr.s_addr = _ipBuffer;
	//for (unsigned int _i = 0; _i < 8; _i++)
	//	_sendToAddr.sin_zero[_i] = 0;

	addressBook.push_back(_sendToAddr);
	addressLengthBook.push_back(_sendToAddrSize);

	// Send the Connection Demand Packet to the server
	if (Network::hr == 0) _nbSendBytes = udpTcpSend(_sendToAddr, (char*)_connectionDemandPacket.c_str(), _connectionDemandPacket.size());
	if (_nbSendBytes == SOCKET_ERROR) Network::hr = SOCKET_ERROR;

	if (Network::hr != 0) {
		std::cerr << "\n ERROR at bool UDP::Connect(std::string _ipAddress, unsigned short _port, bool _openPort) :\n"
			<< WSAGetLastError();
		return false;
	}


	return true;
		if (_nbSendBytes == SOCKET_ERROR) Network::hr = SOCKET_ERROR;
}

int UDP::udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength) {
	return sendto(hostSocket, _bufferToSend, _bufferToSendLength, 0, (SOCKADDR*)&addressBook[_clientID], addressLengthBook[_clientID]);
}

int UDP::udpTcpSend(sockaddr_in _sendToAddr, char* _bufferToSend, const int _bufferToSendLength) {
	return sendto(hostSocket, _bufferToSend, _bufferToSendLength, 0, (SOCKADDR*)&_sendToAddr, sizeof(_sendToAddr));
}

int UDP::udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength) {
	int _nbRecvBytes = 0;

	struct sockaddr_in _senderAddr; // Store sender informations
	int _senderAddrSize = sizeof(_senderAddr);

	_nbRecvBytes = recvfrom(hostSocket, _recvBuffer, _recvBufferLength, 0, (SOCKADDR*)&_senderAddr, &_senderAddrSize);

	// If the address is not store, add it
	unsigned int _i;
	bool _alreadyInVector = false;
	for (_i = 0; _i < addressBook.size(); _i++) {
		if (Network::Compare(addressBook[_i], _senderAddr)) {
			_alreadyInVector = true;
			break;
		}
	}
	if (!_alreadyInVector) {
		addressBook.push_back(_senderAddr);
		addressLengthBook.push_back(_senderAddrSize);
		int test = sizeof(_senderAddr);
		test = test;
	}

	_clientID = _i; // Return the clientID

	return _nbRecvBytes;
}

#pragma endregion //UDP



void Network::Initialize() {
	// Static Variables
	hr = 0;

	WORD wVersionRequested = MAKEWORD(2, 2); // Request Socket Version 2.2
	WSADATA _WSAData; // Receive the information about the Socket Implementation

	if (hr == 0) hr = WSAStartup(wVersionRequested, &_WSAData);

	if (hr != 0) throw hr;
}

void Network::Destruct() {
	for (unsigned int _i = 0; _i < tcp.size(); _i++)
		Protocole(tcp[_i]).~Protocole();

	for (unsigned int _i = 0; _i < udp.size(); _i++)
		Protocole(udp[_i]).~Protocole();

	WSACleanup();
}

void Network::Add(const unsigned int _protocoleType) {
	if (_protocoleType == ProtocoleTypes::TCP || _protocoleType == ProtocoleTypes::BOTH)
		tcp.emplace_back();

	if (_protocoleType == ProtocoleTypes::UDP || _protocoleType == ProtocoleTypes::BOTH)
		udp.emplace_back();
}

bool Network::Compare(sockaddr_in _a, sockaddr_in _b) {
	return (_a.sin_family == _b.sin_family && _a.sin_port == _b.sin_port && _a.sin_addr.S_un.S_addr == _a.sin_addr.S_un.S_addr);
}

sockaddr_in Network::CreateSockaddr_in(unsigned short _family, std::string _ipAddress, unsigned short _port) {
	struct sockaddr_in _returnAddr;
	unsigned long _ipBuffer;

	_returnAddr.sin_family = AF_INET;
	_returnAddr.sin_port = htons(_port);
	inet_pton(AF_INET, _ipAddress.c_str(), &_ipBuffer);
	_returnAddr.sin_addr.s_addr = _ipBuffer;
	for (unsigned int _i = 0; _i < 8; _i++)
		_returnAddr.sin_zero[_i] = 0;

	return _returnAddr;
}