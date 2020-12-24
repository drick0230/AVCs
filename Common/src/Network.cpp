#include "Network.h"

// Static Variables
int Network::hr;

TCP Network::tcp;
UDP Network::udp;

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
UDP::UDP() : Protocole(AF_INET, SOCK_DGRAM, IPPROTO_UDP) {} // Socket as IPV4 and UDP 

bool Protocole::Connect(std::string _ipAddress, std::string _port, bool _openPort) {
	struct addrinfo* _serverInfo = NULL;

	if (Network::hr == 0) Network::hr = getaddrinfo(_ipAddress.c_str(), _port.c_str(), &clientInfo, &_serverInfo); 	// Resolve the server address and port
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

bool Protocole::Host(short _port, bool _openPort) { return Host(std::to_string(_port), _openPort); }
bool Protocole::Host(std::string _port, bool _openPort) {
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

	// Begin listenning on the socket
	if (listen(hostSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error: " << WSAGetLastError() << '\n';
		closesocket(hostSocket);
		throw hostSocket;
	}

	freeaddrinfo(_result);

	if (Network::hr != 0) {
		throw Network::hr;
		return false;
	}

	return true;
}

void Protocole::WaitClientConnection() {
	clientsSocket.emplace_back(INVALID_SOCKET);

	// Accept a client socket
	clientsSocket.back() = accept(hostSocket, NULL, NULL);
	if (clientsSocket.back() == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		throw(clientsSocket.back());
		closesocket(hostSocket);
	}

}

void Protocole::WaitReceive(unsigned int _clientID) {
	const unsigned int _recvBufferLength = 512;
	char _recvBuffer[_recvBufferLength];
	int _nbRecvBytes = 0;
	SOCKET* recvFromSocket = NULL;

	if (_clientID == -1) recvFromSocket = &connectSocket; // Send to the server
	else recvFromSocket = &clientsSocket[_clientID]; // Send to a client

	_nbRecvBytes = recv(*recvFromSocket, _recvBuffer, _recvBufferLength, 0);
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
	}

	if (Network::hr != 0) throw Network::hr;
}

void Protocole::Send(std::string _str) { Send(-1, _str); }
void Protocole::Send(unsigned int _clientID, std::string _str){
	const unsigned int _sendBufferLength = 512;
	int _nbSendBytes = 0;
	SOCKET* _sendToSocket = NULL;

	if (_clientID == -1) _sendToSocket = &connectSocket; // Send to the server
	else _sendToSocket = &clientsSocket[_clientID]; // Send to a client

	_nbSendBytes = send(*_sendToSocket, _str.c_str(), _str.size(), 0);
	if (_nbSendBytes == SOCKET_ERROR) Network::hr = SOCKET_ERROR;
	if (_nbSendBytes != _str.size()) {
		std::cerr << "Only " << _nbSendBytes << " Bytes was send and not " << _str.size() << "Bytes\n";
	}

	if (Network::hr != 0) throw Network::hr;
}



void Network::Initialize() {
	// Static Variables
	hr = 0;
	tcp = TCP();
	udp = UDP();

	WORD wVersionRequested = MAKEWORD(2, 2); // Request Socket Version 2.2
	WSADATA _WSAData; // Receive the information about the Socket Implementation

	if (hr == 0) hr = WSAStartup(wVersionRequested, &_WSAData);

	if (hr != 0) throw hr;
}

void Network::Destruct() {
	Protocole(tcp).~Protocole();
	Protocole(udp).~Protocole();
	WSACleanup();
}