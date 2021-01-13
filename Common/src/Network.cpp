#include "Network.h"

// Static Variables
int Network::hr;

//std::vector<TCP> Network::tcp;
std::vector <UDP> Network::udp;
/*
Protocole::Protocole(int _family, int _sockType, int _protocol) : mySocket(INVALID_SOCKET) {
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
*/

UDP::UDP() : first(0), end(0), mySocket(INVALID_SOCKET), serverAddr(), inUse(), tReceiving(), isReceiving(false) {
	std::lock_guard<std::mutex> LG_inUse(inUse);  // Lock the use of UDP in other threads and unlock it at end of current task

	// Initialize netPacketBuffer with empty NetPacket
	for (size_t _i = 0; _i < netPacketBufferSize; _i++)
		netPacketBuffer[_i] = NULL;
}

UDP::UDP(UDP&& _udp) : first(_udp.first), end(_udp.end), mySocket(INVALID_SOCKET), serverAddr(), inUse(), tReceiving(), isReceiving(false) {
	std::lock_guard<std::mutex> LG_inUse2(_udp.inUse);  // Lock the use of UDP in other threads and unlock it at end of current task
	std::lock_guard<std::mutex> LG_inUse(inUse);  // Lock the use of UDP in other threads and unlock it at end of current task

	// Initialize netPacketBuffer with the other UDP NetPacketBuffer
	for (size_t _i = 0; _i < netPacketBufferSize; _i++) {
		netPacketBuffer[_i] = _udp.netPacketBuffer[_i];
		_udp.netPacketBuffer[_i] = NULL; // prevent them from beiing delete by the destructor of _udp
	}
}


UDP::~UDP() {
	std::lock_guard<std::mutex> LG_inUse(inUse);  // Lock the use of UDP in other threads and unlock it at end of current task
	int _hr = 0;

	// Completely close and stop operation on the Socket
	_hr = shutdown(mySocket, 2);
	_hr = closesocket(mySocket);

	if (_hr != 0) {
		std::cerr << "\n ERROR at ~UDP() :\n"
			<< WSAGetLastError();
		throw _hr;
	}

	isReceiving = false;
	tReceiving.join();

	// Delete netPacketBuffer's NetPackets
	for (size_t _i = 0; _i < netPacketBufferSize; _i++)
		if (netPacketBuffer[_i] != NULL)
		{
			delete netPacketBuffer[_i];
			netPacketBuffer[_i] = NULL;
		}
}

//#pragma region Protocole
//
//// TCP : Receive packets from a specific Client
//// UDP : Receive packets from everyone. Return the ID of the sender.
//unsigned int Protocole::WaitReceive(Packet& _recvPacket, unsigned int _clientID) {
//	int hr = 0;
//	const unsigned int _recvBufferLength = 20000;
//	char _recvBuffer[_recvBufferLength];
//	int _nbRecvBytes = 0;
//
//	_nbRecvBytes = udpTcpReceive(_clientID, _recvBuffer, _recvBufferLength);
//
//	if (_nbRecvBytes == SOCKET_ERROR) hr = SOCKET_ERROR;
//	if (_nbRecvBytes > 0 && hr == 0) {
//		_recvPacket = Packet(_nbRecvBytes);
//		_recvPacket.add(_recvBuffer, _nbRecvBytes);
//		_recvPacket.move(0);
//		//// Receive something
//		//std::cout << "Host receive : " << _nbRecvBytes << " Bytes\n";
//		//std::cout << "Received Bytes are : \n";
//
//		//// Print datas
//		//unsigned int _i = 0;
//		//while (_recvBuffer[_i] != '\0' && _i < _nbRecvBytes) {
//		//	std::cout << _recvBuffer[_i];
//		//	_i++;
//		//}
//		//std::cout << '\n';
//	}
//
//	if (hr != 0) {
//		std::cerr << "\n ERROR at unsigned int Protocole::WaitReceive(Packet& _recvPacket, unsigned int _clientID) :\n"
//			<< WSAGetLastError();
//		throw hr;
//	}
//	return _clientID;
//}
//
//void Protocole::Send(std::string _str) { Send(-1, _str); }
//void Protocole::Send(unsigned int _clientID, std::string _str) { Send(_clientID, (char*)_str.c_str(), _str.size()); }
//
//void Protocole::Send(Packet& _packetToSend) { Send(-1, _packetToSend); }
//void Protocole::Send(unsigned int _clientID, Packet& _packetToSend) { Send(_clientID, _packetToSend.data(), _packetToSend.size()); }
//
//void Protocole::Send(unsigned int _clientID, char* _bufferToSend, int _bufferToSendLength) {
//	int hr = 0;
//
//	int _nbSendBytes = 0;
//
//	_nbSendBytes = udpTcpSend(_clientID, _bufferToSend, _bufferToSendLength);
//
//	if (_nbSendBytes == SOCKET_ERROR) hr = SOCKET_ERROR;
//	if (_nbSendBytes != _bufferToSendLength) {
//		std::cerr << "Only " << _nbSendBytes << " Bytes was send and not " << _bufferToSendLength << "Bytes\n";
//	}
//
//	if (hr != 0) {
//		std::cerr << "\n ERROR at void Protocole::Send(unsigned int _clientID, char* _bufferToSend, int _bufferToSendLength) :\n"
//			<< WSAGetLastError();
//		throw hr;
//	}
//}
//void Protocole::Send(SOCKET _clientSocket, std::string _ipAddress, unsigned short _port, char* _bufferToSend, const int _bufferToSendLength) {
//
//}
//#pragma endregion //Protocole
/*
#pragma region TCP
bool TCP::Host(unsigned short _port) { return Host(std::to_string(_port)); }
bool TCP::Host(std::string _port) {
	int hr = 0;

	struct addrinfo* _result = NULL;

	if (hr == 0) hr = getaddrinfo(NULL, _port.c_str(), &serverInfo, &_result); 	// Resolve the server address and port
	if (hr == 0) mySocket = socket(_result->ai_family, _result->ai_socktype, _result->ai_protocol); // Create a SOCKET for connecting to server
	if (mySocket == INVALID_SOCKET) {
		hr = INVALID_SOCKET;
		throw mySocket;
	}

	// Bind the socket
	if (hr == 0) hr = bind(mySocket, _result->ai_addr, (int)_result->ai_addrlen);
	if (hr == SOCKET_ERROR) {
		std::cerr << "bind failed with error : " << WSAGetLastError() << '\n';
		closesocket(mySocket);
		mySocket = INVALID_SOCKET;
	}

	tcpListen(); // Only for TCP

	freeaddrinfo(_result);

	if (hr != 0) {
		throw hr;
		return false;
	}

	return true;
}

unsigned int TCP::Connect(std::string _ipAddress, unsigned short _port) {
	int hr = 0;

	struct addrinfo* _serverInfo = NULL;

	if (hr == 0) hr = getaddrinfo(_ipAddress.c_str(), std::to_string(_port).c_str(), &clientInfo, &_serverInfo); 	// Resolve the server address and port
	if (hr == 0) mySocket = socket(_serverInfo->ai_family, _serverInfo->ai_socktype, _serverInfo->ai_protocol); // Create a SOCKET for connecting to server
	if (mySocket == INVALID_SOCKET) {
		hr = INVALID_SOCKET;
		throw mySocket;
	}

	// Connect to server
	if (hr == 0) hr = connect(mySocket, _serverInfo->ai_addr, (int)_serverInfo->ai_addrlen);
	if (hr == SOCKET_ERROR) {
		throw mySocket;
		closesocket(mySocket);
		mySocket = INVALID_SOCKET;
	}



	if (mySocket == INVALID_SOCKET) {
		//Unable to connect to server!
		return false;
	}

	freeaddrinfo(_serverInfo);

	if (hr != 0) throw hr;

	return true;
}

void TCP::tcpListen() {
	// Begin listenning on the socket
	if (listen(mySocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Listen failed with error: " << WSAGetLastError() << '\n';
		closesocket(mySocket);
		throw mySocket;
	}
}

unsigned int TCP::WaitClientConnection() {
	unsigned int _returnClientID = 0;
	struct sockaddr_in _senderAddr; // Store sender informations
	int _senderAddrSize = sizeof(_senderAddr);

	_returnClientID = clientsSocket.size();
	clientsSocket.emplace_back(INVALID_SOCKET);

	// Accept a client socket
	clientsSocket.back() = accept(mySocket, (sockaddr*)&_senderAddr, &_senderAddrSize);
	if (clientsSocket.back() == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		throw(clientsSocket.back());
		closesocket(mySocket);
	}
	else {
		// Add info in addressBook and portBook
		unsigned short _clientPort;
		addressBook.push_back(Network::GetSocketInfo(_clientPort, clientsSocket.back()));
		portBook.push_back(_clientPort);
	}

	return _returnClientID;
}

int TCP::udpTcpSend(unsigned int _clientID, char* _bufferToSend, const int _bufferToSendLength) {
	SOCKET* _sendToSocket = NULL;

	if (_clientID == -1) _sendToSocket = &mySocket; // Send to the server
	else _sendToSocket = &clientsSocket[_clientID]; // Send to a client

	return send(*_sendToSocket, _bufferToSend, _bufferToSendLength, 0);
}

int TCP::udpTcpReceive(unsigned int& _clientID, char* _recvBuffer, const int _recvBufferLength) {
	SOCKET* recvFromSocket = NULL;

	if (_clientID == -1) recvFromSocket = &mySocket; // Send to the server
	else recvFromSocket = &clientsSocket[_clientID]; // Send to a client

	return recv(*recvFromSocket, _recvBuffer, _recvBufferLength, 0);
}

std::string TCP::GetClientInfo(unsigned short& _returnPort, unsigned int _clientID) { return Network::GetSocketInfo(_returnPort, clientsSocket[_clientID]); }
#pragma endregion //TCP
*/
#pragma region UDP
/// Private
size_t UDP::FoundNetPacket(unsigned int _clientID, unsigned int _packetID) {
	unsigned int _posInBuf;
	for (_posInBuf = 0; _posInBuf < netPacketBufferSize; _posInBuf++)
		if (netPacketBuffer[_posInBuf]->capacity() != 0)
			if (netPacketBuffer[_posInBuf]->clientID == _clientID)
				if (netPacketBuffer[_posInBuf]->packetID == _packetID)
					return _posInBuf;

	return netPacketBufferSize;
}

void UDP::Emplace_back(unsigned int _packetID, unsigned int _clientID, unsigned char _DGRAMSize_T) {
	MoveEnd();

	if (end == first) {
		// first data is too old, delete it
		if (netPacketBuffer[first] != NULL)
			delete netPacketBuffer[first];
		MoveFirst();
	}

	netPacketBuffer[end - 1] = new RcvNetPacket(_packetID, _clientID, _DGRAMSize_T); // Create a new NetPacket at last position
}

RcvNetPacket* UDP::Pop_front() {
	std::lock_guard<std::mutex> LG_inUse(inUse);  // Lock the use of UDP in other threads and unlock it at end of current task (return)

	RcvNetPacket* _return = NULL;

	if (first != end) // The buffer is not empty
		if (netPacketBuffer[first]->nbRcvDGRAM == netPacketBuffer[first]->nbRcvDGRAM_T) {
			// the NetPacket is ready to be pop (Received all his Datagram)
			_return = netPacketBuffer[first]; // Return the NetPacket pointer
			netPacketBuffer[first] = NULL; // Prevent the NetPacket from being delete automatically

			MoveFirst();
		}

	return _return;
}

void UDP::MoveFirst() {
	first++;
	if (first > netPacketBufferSize)
		first = 0;
}
void UDP::MoveEnd() {
	end++;
	if (end == netPacketBufferSize)
		end = 0;
}

/// Public
bool UDP::Bind(std::string _ipAddress, unsigned short _port) {
	// Convert from String to Ip Address as ULONG
	unsigned long _ipBuffer;
	inet_pton(AF_INET, _ipAddress.c_str(), &_ipBuffer);

	return Bind(_ipBuffer, _port);
}
bool UDP::Bind(unsigned long _ipAddress, unsigned short _port) {
	int hr = 0;

	// Create the socket with the _ipAddress and _port
	serverAddr = Network::CreateSockaddr_in(AF_INET, _ipAddress, _port);
	mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Socket as IPV4, Datagram and UDP

	// Bind the socket
	if (hr == 0) hr = bind(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (hr == SOCKET_ERROR) {
		std::cerr << "bind failed with error : " << WSAGetLastError() << '\n';
		closesocket(mySocket);
		mySocket = INVALID_SOCKET;
	}

	if (hr != 0) {
		throw hr;
		return false;
	}

	return true;
}

unsigned int UDP::AddToBook(std::string _ipAddress, unsigned short _port) {
	// Create the sockaddr of the destination
	struct sockaddr_in _sendToAddr = Network::CreateSockaddr_in(AF_INET, _ipAddress, _port);

	// Add it in the Book
	sockAddressBook.push_back(_sendToAddr);
	addressBook.push_back(_ipAddress);
	portBook.push_back(_port);

	return sockAddressBook.size() - 1; // Return the clientID
}
bool UDP::IsInBook(std::string _ipAddress, unsigned short _port) {
	for (unsigned int _i = 0; _i < addressBook.size(); _i++)
		if (addressBook[_i] == _ipAddress)
			if (portBook[_i] == _port)
				return true;

	return false;
}

void UDP::BeginReceiving() {
	if (isReceiving) {
		std::cerr << "\nvoid UDP::BeginReceiving() Already called\n";
		throw "void UDP::BeginReceiving() Already called";
		return;
	}

	tReceiving = std::thread([this] {
		while (isReceiving) {
			std::lock_guard<std::mutex> LG_inUse(inUse); // Lock the use of UDP in other threads and unlock it at end of current task (while loop)
			int hr = 0;

			char _rcvDGRAM[NetPacket::DGRAM_SIZE];
			int _nbRecvBytes = 0;

			struct sockaddr_in _senderAddr; // Store sender informations
			int _senderAddrSize = sizeof(_senderAddr);

			_nbRecvBytes = recvfrom(mySocket, _rcvDGRAM, NetPacket::DGRAM_SIZE, 0, (SOCKADDR*)&_senderAddr, &_senderAddrSize);

			if (_nbRecvBytes != SOCKET_ERROR) {
				// If the address is not store, add it
				unsigned int _clientID;
				bool _alreadyInVector = false;
				for (_clientID = 0; _clientID < sockAddressBook.size(); _clientID++) {
					if (Network::Compare(sockAddressBook[_clientID], _senderAddr)) {
						_alreadyInVector = true;
						break;
					}
				}
				if (!_alreadyInVector) {
					unsigned short _senderPort;
					sockAddressBook.push_back(_senderAddr);
					addressBook.push_back(Network::GetSocketInfo(_senderPort, _senderAddr));
					portBook.push_back(_senderPort);

					int test = sizeof(_senderAddr);
					test = test;
				}

				const unsigned char _packetID = _rcvDGRAM[0];
				const unsigned char _DGRAMid = _rcvDGRAM[1];
				const unsigned char _DGRAMsize = _rcvDGRAM[2];

				size_t _posInBuf = FoundNetPacket(_clientID, _packetID);
				if (_posInBuf != netPacketBufferSize) {
					// The NetPacket already exist. Add the DGRAM
					netPacketBuffer[_posInBuf]->AddDGRAM(_rcvDGRAM, _nbRecvBytes);
				}
				else {
					// Create New NetPacket in Buffer at last position and add the DGRAM
					Emplace_back(_packetID, _clientID, _DGRAMsize);
					netPacketBuffer[end - 1]->AddDGRAM(_rcvDGRAM, _nbRecvBytes);
				}
			}
			else hr = SOCKET_ERROR; // Error

			if (hr != 0) {
				std::cerr << "\n ERROR at unsigned int Protocole::WaitReceive(Packet& _recvPacket, unsigned int _clientID) :\n"
					<< WSAGetLastError();
				throw hr;
			}
		}
		});
}

void UDP::Send(unsigned int _clientID, SendNetPacket& _netPacket) {
	int _hr = 0;
	int _nbSendBytes = 0;
	char* _datasToSend;
	int _datasToSendLength;

	if (_netPacket.size() < (NetPacket::DGRAM_SIZE_WO_HEAD)) {
		// Can send directly the NetPacket
		_datasToSendLength = NetPacket::HEAD_SIZE + _netPacket.size();
		_datasToSend = new char[_datasToSendLength];

		// Header of the NetPacket
		_datasToSend[0] = _netPacket.packetID;  // NetPacket ID
		_datasToSend[1] = 0; // iDGRAM
		_datasToSend[2] = _netPacket.size(); // NetPacket size in byte

		// Datas of the NetPacket
		size_t _iNetPacketData = 0; // Increments of the NetPacket data
		for (size_t _iDataToSend = NetPacket::HEAD_SIZE; _iDataToSend < _netPacket.size() + NetPacket::HEAD_SIZE; _iDataToSend++, _iNetPacketData++)
			_datasToSend[_iDataToSend] = _netPacket.data()[_iNetPacketData];

		_nbSendBytes = sendto(mySocket, _datasToSend, _datasToSendLength, 0, (SOCKADDR*)&sockAddressBook[_clientID], sizeof(sockAddressBook[_clientID]));

		// Check if error are detected
		if (_nbSendBytes == SOCKET_ERROR)
			_hr = SOCKET_ERROR;
		else if (_nbSendBytes != _datasToSendLength) {
			std::cerr << "Only " << _nbSendBytes << " Bytes was send and not " << _datasToSendLength << "Bytes\n";
		}

		if (_hr != 0) {
			std::cerr << "\n ERROR at void Protocole::Send(unsigned int _clientID, char* _bufferToSend, int _bufferToSendLength) :\n"
				<< WSAGetLastError();
			throw _hr;
		}
	}

	//unsigned char _nbDGRAM = _netPacket.size() / (NetPacket::maxDGRAMSize - _custmHeaderSize) + 1; // Number of datagram to send

	//for (unsigned char _iDGRAM = 0; _iDGRAM < _nbDGRAM; _iDGRAM++) {

	//}

}

std::string UDP::GetClientInfo(unsigned short& _returnPort, unsigned int _clientID) { return Network::GetSocketInfo(_returnPort, sockAddressBook[_clientID]); }
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
	WSACleanup();
}

void Network::Add(const unsigned int _protocoleType, const unsigned char _nbToAdd) {
	//if (_protocoleType == ProtocoleTypes::TCP || _protocoleType == ProtocoleTypes::BOTH)
	//	for (unsigned int _i = 0; _i < _nbToAdd; _i++)
	//		tcp.emplace_back();

	if (_protocoleType == ProtocoleTypes::UDP || _protocoleType == ProtocoleTypes::BOTH)
		for (unsigned int _i = 0; _i < _nbToAdd; _i++)
			udp.emplace_back();
}

bool Network::Compare(sockaddr_in _a, sockaddr_in _b) {
	return (_a.sin_family == _b.sin_family && _a.sin_port == _b.sin_port && _a.sin_addr.S_un.S_addr == _a.sin_addr.S_un.S_addr);
}

sockaddr_in Network::CreateSockaddr_in(unsigned short _family, std::string _ipAddress, unsigned short _port) {
	// Convert from String to Ip Address as ULONG
	unsigned long _ipBuffer;
	inet_pton(AF_INET, _ipAddress.c_str(), &_ipBuffer);

	return CreateSockaddr_in(_family, _ipBuffer, _port);
}

sockaddr_in Network::CreateSockaddr_in(unsigned short _family, unsigned long _ipAddress, unsigned short _port) {
	struct sockaddr_in _returnAddr;

	_returnAddr.sin_family = AF_INET;
	_returnAddr.sin_port = htons(_port); // Convert to big-endian
	_returnAddr.sin_addr.s_addr = _ipAddress;
	for (unsigned int _i = 0; _i < 8; _i++)
		_returnAddr.sin_zero[_i] = 0;

	return _returnAddr;
}

std::string Network::GetSocketInfo(unsigned short& _returnPort, SOCKET _socket) {
	int hr = 0;
	sockaddr_in _socketAddr;
	int _socketAddrSize = sizeof(_socketAddr);

	// Socket to sockaddr
	if (hr == 0) hr = getpeername(_socket, (SOCKADDR*)&_socketAddr, &_socketAddrSize);

	if (hr != 0) {
		std::cerr << "\n ERROR at std::string Network::GetSocketInfo(unsigned short& _returnPort, SOCKET _socket) :\n"
			<< WSAGetLastError();
		return "ERROR";
	}

	return GetSocketInfo(_returnPort, _socketAddr);
}
std::string Network::GetSocketInfo(unsigned short& _returnPort, sockaddr_in _socket) {
	int hr = 0;

	char _sBuff[30];
	unsigned long _sBuffSize = sizeof(_sBuff);

	std::vector<std::string> _splitStr;

	if (hr == 0) hr = WSAAddressToStringA((SOCKADDR*)&_socket, sizeof(_socket), NULL, _sBuff, &_sBuffSize);
	if (hr == 0) _splitStr = split(std::string(_sBuff), ':');


	if (hr == 0) _returnPort = myParse<unsigned short>(_splitStr[1]);

	if (hr != 0) {
		std::cerr << "\n ERROR at std::string TCP::GetClientInfo(unsigned short& _returnPort, unsigned int _clientID) :\n"
			<< WSAGetLastError();
		return "ERROR";
	}

	return _splitStr[0];
}