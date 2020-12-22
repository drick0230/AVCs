#include "server.h"

short Server::FindRoomId(string name)
{
	for (int i = 0; i < listeRoom.size(); i++)
	{
		if (listeRoom[i].getName() == name) return i;
	}
	return -1;
}

Server::Server(unsigned short port) : endListener(false), endCom(false),
	tListener(&Server::fListener, this, port), tCom(&Server::fCom,this)
{
	
}

Server::~Server()
{
	//Libération des thread
	unique_lock<mutex> lListener(mListener);
	endListener = true;
	listener.close();
	lListener.unlock();
	unique_lock<mutex> lCom(mCom);
	endCom = true;
	lCom.unlock();
	if (tListener.joinable())tListener.join();
	if (tCom.joinable())tCom.join();
	//Libération de la liste des connection
	for (int i = 0; i < listeConnection.size(); i++)
	{
		listeConnection[i]->socket.disconnect();
		delete listeConnection[i];
	}
	listeConnection.clear();
}

void Server::fListener(unsigned short port)
{
	unique_lock<mutex> lListener(mListener);
	listener.listen(port);
	lListener.unlock();

	while (true)
	{
		lListener.lock();
		if (endListener)
			break;
		lListener.unlock();
		//ajout de nouveau contact
		TCPServerConnection* newConnection;
		newConnection = new TCPServerConnection;
		if (listener.accept(newConnection->socket) != sf::Socket::Status::Error)
		{
			lListener.lock();
			newConnection->state = 1;
			listeConnection.push_back(newConnection);
			lListener.unlock();
		}

	}

	listener.close();
}

void Server::fCom()
{
	while (true)
	{
		unique_lock <mutex> lCom(mCom);
		if (endCom)break;
		for (int i=0; i < listeConnection.size(); i++)
		{
			if (listeConnection[i]->state > 0)
			{
				sf::Packet packet;
				sf::Socket::Status state (sf::Socket::Status::Partial);
				listeConnection[i]->socket.setBlocking(false);
				while (state == sf::Socket::Status::Partial) state = listeConnection[i]->socket.receive(packet);
				if (state == sf::Socket::Status::Disconnected) listeConnection[i]->state = 0;
				else if (state == sf::Socket::Status::Error)throw string("probleme connection");
				else if (state == sf::Socket::Status::Done)analysePacket(packet, i);
			}
		}
		lCom.unlock();
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

void Server::analysePacket(sf::Packet packet, int id)
{
	UINT8 commande;
	sf::Packet reponse;
	packet >> commande;
	switch (commande)
	{
	case ServerCommand::createRoom:
	{

		string nom;
		packet >> nom;

		unique_lock <mutex> lRoom(mRoom);
		bool present = FindRoomId("nom") >= 0;
		if (present) reponse << false;
		else {
			listeRoom.push_back(Room_server(nom));
			reponse << true;
		}
		lRoom.unlock();
		listeConnection[id]->socket.send(reponse);
	}
	break;
	case ServerCommand::ExitRoom:
	{
		string room;
		string pseudo = "";
		packet >> room;
		//recher utilisateur
		unique_lock <mutex> lRoom(mRoom);
		int roomid = FindRoomId(room);
		bool presence = roomid >= 0;
		if (presence)pseudo = listeRoom[roomid].findPseudoWithSocket(&listeConnection[id]->socket);
		presence = pseudo != "";

		//envoie réponse
		if (presence) reponse << true;
		else reponse << false;
		listeConnection[id]->socket.send(reponse);

		//process
		if(presence) listeRoom[roomid].removeUser(pseudo);
	}
		break;
	case ServerCommand::joinRoom:
	{
		string roomName;
		unsigned short Roomid=0;
		bool presence = false;
		packet >> roomName;

		user new_user;
		packet >> new_user.pseudo;
		new_user.port = listeConnection[id]->socket.getRemotePort();
		new_user.ip = listeConnection[id]->socket.getRemoteAddress();
		//recherche de la salle
		unique_lock <mutex> lRoom(mRoom);
		Roomid = FindRoomId(roomName);
		presence = Roomid >= 0;
		//test replicat
		bool replicat = false;
		if(presence) replicat = listeRoom[Roomid].testReplicatAdresse(new_user.ip, new_user.port);
		//réponse
		bool proceed = presence && !replicat;
		if (proceed)reponse << true;
		else reponse << false;
		listeConnection[id]->socket.send(reponse);
		//ajout si possible
		if (proceed )listeRoom[Roomid].addUser(new_user, &listeConnection[id]->socket);
		lRoom.unlock();
	}
		break;
	case ServerCommand::print:
	{
		string message;
		packet >> message;
		cout << message;
	}
		reponse << true;
		listeConnection[id]->socket.send(reponse);
		break;
	default:
		reponse << false;
		listeConnection[id]->socket.send(reponse);
		break;
	}
	
}

void Server::print()
{
	unique_lock<mutex> lListener(mListener);
	unique_lock<mutex> lRoom(mRoom);
	cout << "listeConnection" << endl;
	for (int i = 0; i < listeConnection.size(); i++)cout << listeConnection[i]->socket.getRemoteAddress() << ":" << listeConnection[i]->socket.getRemotePort() << "   " << listeConnection[i]->state << endl;
	cout << "listeRoom:" << endl;
	for (int i = 0; i < listeRoom.size(); i++)listeRoom[i].Room_server::printS();
}