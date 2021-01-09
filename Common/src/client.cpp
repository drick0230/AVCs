#include "client.h"

short Client::FindRoomId(string name)
{
	for (int i = 0; i < listeRoom.size(); i++)
	{
		if (listeRoom[i]->name == name) return i;
	}
	return -1;
}

Client::Client(std::string _serverIP, unsigned short _serverPort) : endServerCom(false), udp()
{
	udp.Bind(INADDR_ANY, 0);

	// Connect to Server (Keep Alive)
	udp.AddToBook(_serverIP, _serverPort);

	tKeepAlives.emplace_back(&Client::KeepAlive, this, 0, 5000);
	tKeepAlives.back().detach();

	// Server communication thread
	tServerCom = std::thread(&Client::fServerCom, this);
	tServerCom.detach();
}

Client::~Client()
{
	//libération du thread
	vector <string> roomsName;
	unique_lock<mutex> lRoom(mRoom);
	for (int i = 0; i < listeRoom.size(); i++) roomsName.push_back(listeRoom[i]->name);
	lRoom.unlock();
	for (int i = 0; i < roomsName.size(); i++)
	{
		exitRoom(roomsName[i]);
	}
	unique_lock<mutex> lSocket(mSocket);
	//fin du thread
	endServerCom = true;
	lSocket.unlock();
	if (tServerCom.joinable())tServerCom.join();
	//libération de la connection


	lSocket.lock();
	//socket.disconnect();

	//destruction des salles restante
	for (int i = 0; i < listeRoom.size(); i++)
	{
		delete listeRoom[i];
	}
	listeRoom.clear();
}

bool Client::joinRoom(string roomName, string pseudo)
{
	unique_lock<mutex> lSocket(mSocket);
	unique_lock<mutex> lRoom(mRoom);

	Packet demande;
	Packet reponse;

	demande << ServerCommand::joinRoom << roomName << pseudo;

	udp.Send(demande);

	bool retour = true;
	//reponse >> retour;
	if (retour)	listeRoom.push_back(new Room_client(roomName));
	return retour;

}
bool Client::createRoom(string roomName)
{
	unique_lock<mutex> lSocket(mSocket);
	Packet demande;
	Packet reponse;

	demande << ServerCommand::createRoom << roomName;

	udp.Send(demande);
	//tcp.WaitReceive(reponse);

	bool retour = true;
	//reponse >> retour;

	return retour;
}
bool Client::exitRoom(string roomName)
{
	unique_lock<mutex> lSocket(mSocket);
	Packet demande;
	Packet reponse;

	demande << ServerCommand::ExitRoom << roomName;

	udp.Send(demande);
	//tcp.WaitReceive(reponse);
	lSocket.unlock();


	bool retour = true;
	//reponse >> retour;
	return retour;
}

void Client::fServerCom()
{
	while (true)
	{
		Packet _recvPacket;
		udp.WaitReceive(_recvPacket);
		analysePacket(_recvPacket);
	}
}

void Client::KeepAlive(unsigned int _clientID, unsigned int _ms) {
	while (1) {
		Packet _packet;
		_packet << std::string("KEEP_ALIVE");
		udp.Send(_clientID, _packet);
		std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
	}
}

void Client::analysePacket(Packet _packet)
{
	UINT8 commande;
	Packet reponse;
	_packet >> commande;
	switch (commande)
	{
	case ClientCommand::addUser:
	{
		string room;
		user new_user;
		std::string _userIpAddress;
		unsigned short _userPort;

		_packet >> room;
		_packet >> new_user.pseudo;
		_packet >> _userIpAddress;
		_packet >> _userPort;


		unique_lock <mutex> lRoom(mRoom);
		short Roomid = FindRoomId(room);
		bool presence = Roomid >= 0;
		if (presence) {
			new_user.id = listeRoom[Roomid]->udp.AddToBook(_userIpAddress, _userPort);

			listeRoom[Roomid]->tKeepAlives.emplace_back(&Room_client::KeepAlive, listeRoom[Roomid], new_user.id, 5000);
			listeRoom[Roomid]->tKeepAlives.back().detach();

			listeRoom[Roomid]->addUser(new_user);
		}
		lRoom.unlock();

	}
	break;

	case ClientCommand::username:
	{
		string room;
		std::string pseudo;

		_packet >> room;
		_packet >> pseudo;

		unique_lock <mutex> lRoom(mRoom);
		unsigned short Roomid = FindRoomId(room);
		bool presence = (Roomid >= 0);
		if (presence)
		{
			listeRoom[Roomid]->setIdentity(pseudo);
		}
		lRoom.unlock();
	}
	break;
	case ClientCommand::removeUser:
	{
		string room;
		string username;

		_packet >> room;
		_packet >> username;

		unique_lock <mutex> lRoom(mRoom);
		short roomId = FindRoomId(room);
		if (roomId >= 0)
		{
			if (listeRoom[roomId]->getPseudo() == username)
			{
				listeRoom[roomId] = listeRoom[listeRoom.size() - 1];
				delete listeRoom[listeRoom.size() - 1];
				listeRoom.pop_back();
			}
			else listeRoom[roomId]->removeUser(username);
		}
		lRoom.unlock();

	}
	break;
	}
}

void Client::print()
{
	for (int i = 0; i < listeRoom.size(); i++)
	{
		listeRoom[i]->print();
	}
}

void Client::send(string room, Packet packet)
{
	short id = FindRoomId(room);
	listeRoom[id]->send(packet);
}