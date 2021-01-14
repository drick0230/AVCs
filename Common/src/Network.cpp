#include "Network.h"

// Static Variables
std::vector <UDP> Network::udp;


#pragma region UDP
/// Constructor/Destructor
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

void UDP::Send(unsigned int _clientID, char* _bytes, const size_t _bytesSize, const unsigned char _packetID, const unsigned char _DGRAMid, const unsigned char _nbDGRAM_T) {
	int _hr = 0;

	const int _DGRAMSize = NetPacket::HEAD_SIZE + _bytesSize; // Size of Datagram to send
	char* _DGRAM = new char[_DGRAMSize]; // Datagram to send
	memset(_DGRAM, 0, _DGRAMSize);// Init _DGRAM with zeros

	int _sentDGRAMSize;  // Size of Datagram to sent (May not be equal to _DGRAMSize)

	// Write NetPacket's custom head
	_DGRAM[0] = _packetID;
	_DGRAM[1] = _DGRAMid;
	_DGRAM[2] = _nbDGRAM_T;

	// Write NetPacket's datas 
	for (size_t _iDGRAM = NetPacket::HEAD_SIZE, _iNetPacket = 0; _iNetPacket < _bytesSize; _iDGRAM++, _iNetPacket++)
		_DGRAM[_iDGRAM] = _bytes[_iNetPacket];

	// Send DGRAM
	_sentDGRAMSize = sendto(mySocket, _DGRAM, _DGRAMSize, 0, (SOCKADDR*)&sockAddressBook[_clientID], sizeof(sockAddressBook[_clientID]));

	// Check if errors are detected
	if (_sentDGRAMSize == SOCKET_ERROR)
		_hr = SOCKET_ERROR;
	else if (_sentDGRAMSize != _DGRAMSize) {
		std::cerr << "Only " << _sentDGRAMSize << " Bytes was send and not " << _DGRAMSize << "Bytes\n";
	}

	if (_hr != 0) {
		std::cerr << "\n ERROR at void Protocole::Send(unsigned int _clientID, char* _bufferToSend, int _bufferToSendLength) :\n"
			<< WSAGetLastError();
		throw _hr;
	}
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
			std::lock_guard<std::mutex> LG_inUse(inUse);  // Lock the use of UDP in other threads and unlock it at end of current task
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
	if (_netPacket.size() < (NetPacket::DGRAM_SIZE_WO_HEAD)) // The NetPacket can be send in one DGRAM
		Send(_clientID, _netPacket.data(), _netPacket.size(), _netPacket.packetID, 0, 1);
	else { // The NetPacket need to be send in multiple DGRAMs
		unsigned char _nbDGRAM_T = _netPacket.size() / NetPacket::DGRAM_SIZE_WO_HEAD + 1; // Number of DGRAMs to send

		for (unsigned char _DGRAMid; _DGRAMid < _nbDGRAM_T; _DGRAMid++) {
			Send(_clientID, _netPacket.data(_DGRAMid * ), _netPacket.size(), _netPacket.packetID, 0, 1);
		}
	}

	//unsigned char _nbDGRAM = _netPacket.size() / (NetPacket::maxDGRAMSize - _custmHeaderSize) + 1; // Number of datagram to send

	//for (unsigned char _iDGRAM = 0; _iDGRAM < _nbDGRAM; _iDGRAM++) {

	//}

}
#pragma endregion //UDP



void Network::Initialize() {
	// Static Variables
	int _hr = 0;

	WORD wVersionRequested = MAKEWORD(2, 2); // Request Socket Version 2.2
	WSADATA _WSAData; // Receive the information about the Socket Implementation

	if (_hr == 0) _hr = WSAStartup(wVersionRequested, &_WSAData);

	if (_hr != 0) throw _hr;
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