#include "client.h"

short Client::FindRoomId(string name)
{
	for (int i = 0; i < listeRoom.size(); i++)
	{
		if (listeRoom[i]->name == name) return i;
	}
	return -1;
}

Client::Client(sf::IpAddress ip, unsigned short port): endServerCom(false), tServerCom(&Client::fServerCom,this)
{
	if (socket.connect(ip, port, sf::milliseconds(5000)) == sf::Socket::Status::Error)std::cout << "error" << endl;
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
	socket.disconnect();

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

	sf::Packet demande;
	sf::Packet reponse;

	demande << ServerCommand::joinRoom << roomName << pseudo;

	socket.setBlocking(true);
	socket.send(demande);
	socket.receive(reponse);

	bool retour;
	reponse >> retour;
	if(retour)	listeRoom.push_back(new Room_client(roomName));
	return retour;

}
bool Client::createRoom(string roomName)
{
	unique_lock<mutex> lSocket(mSocket);
	sf::Packet demande;
	sf::Packet reponse;

	demande << ServerCommand::createRoom << roomName;

	socket.setBlocking(true);
	socket.send(demande);
	socket.receive(reponse);

	bool retour;
	reponse >> retour;

	return retour;
}
bool Client::exitRoom(string roomName)
{
	unique_lock<mutex> lSocket(mSocket);
	sf::Packet demande;
	sf::Packet reponse;

	demande << ServerCommand::ExitRoom << roomName;

	socket.setBlocking(true);
	socket.send(demande);
	socket.receive(reponse);
	lSocket.unlock();


	bool retour;
	reponse >> retour;
	return retour;
}

void Client::fServerCom()
{
	while (true)
	{
		unique_lock <mutex> lServerCom(mSocket);
		if (endServerCom)break;
			
		sf::Packet packet;
		sf::Socket::Status state ( sf::Socket::Status::Partial);
		socket.setBlocking(false);
		while (state == sf::Socket::Status::Partial) state = socket.receive(packet);
		if (state == sf::Socket::Status::Error)throw string("probleme connection");
		else if (state == sf::Socket::Status::Done)analysePacket(packet);

		lServerCom.unlock();
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

void Client::analysePacket(sf::Packet packet)
{
	UINT8 commande;
	sf::Packet reponse;
	packet >> commande;
	switch (commande)
	{
	case ClientCommand::addUser :
		{
		string room;
		user new_user;
		UINT32 intIp;

		packet >> room;
		packet >> new_user.pseudo;
		packet >> intIp;
		new_user.ip = sf::IpAddress(intIp);
		packet >> new_user.port;

		unique_lock <mutex> lRoom(mRoom);
		short Roomid = FindRoomId(room);
		bool presence = Roomid >= 0;
		if (presence)listeRoom[Roomid]->addUser(new_user);
		lRoom.unlock();

		}
		break;

	case ClientCommand::username :
		{
		string room;
		user identity;
		sf::Uint32 ipInt;

		packet >> room;
		packet >> identity.pseudo;
		packet >> ipInt;
		packet >> identity.port;


		identity.ip = sf::IpAddress(ipInt);

		unique_lock <mutex> lRoom(mRoom);
		unsigned short Roomid = 0;
		bool presence = false;
		for (; Roomid < listeRoom.size(); Roomid++)
		{
			if (listeRoom[Roomid]->name == room)
			{
				presence = true;
				break;
			}
		}
		if (presence)
		{
			listeRoom[Roomid]->setIdentity(identity);
		}
		lRoom.unlock();
		}
		break;
	case ClientCommand::removeUser:
		{
			string room;
			string username;

			packet >> room;
			packet >> username;

			unique_lock <mutex> lRoom(mRoom);
			short roomId = FindRoomId(room);
			if (roomId >= 0)
			{
				if (listeRoom[roomId]->getPseudo() == username)
				{
					listeRoom[roomId] = listeRoom[listeRoom.size()-1];
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

void Client::send(string room, sf::Packet packet)
{
	short id = FindRoomId(room);
	listeRoom[id]->send(packet);
}