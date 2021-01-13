#include "server.h"

short Server::FindRoomId(string name)
{
	for (int i = 0; i < listeRoom.size(); i++)
	{
		if (listeRoom[i].getName() == name) return i;
	}
	return -1;
}

Server::Server(std::string _localNetworkIP, unsigned short port) : tcp(), localNetworkIP(_localNetworkIP){
	tcp.Host(port);

	tListener = std::thread(&Server::fListener, this);
	tListener.detach();
}

Server::~Server()
{
}

void Server::fListener()
{
	while (true)
	{
		unsigned int _clientID = tcp.WaitClientConnection();
		clientsThreads.emplace_back(&Server::fCom, this, _clientID);
		clientsThreads.back().detach();
	}

	//listener.close();
}

void Server::fCom(unsigned int _clientID)
{
	while (true)
	{
		Packet _recvPacket;
		tcp.WaitReceive(_recvPacket, _clientID);
		analysePacket(_recvPacket, _clientID);
	}
}

void Server::analysePacket(Packet& _packet, unsigned int _clientId)
{
	unsigned char commande;
	Packet reponse;
	_packet >> commande;
	switch (commande)
	{
	case ServerCommand::createRoom:
	{

		std::string nom;
		_packet >> nom;

		unique_lock <mutex> lRoom(mRoom);
		bool present = FindRoomId("nom") >= 0;
		if (present) reponse << false;
		else {
			listeRoom.push_back(Room_server(nom, this));
			listeRoom.back().pTCP = &tcp;
			reponse << true;
		}
		lRoom.unlock();
		//tcp.Send(_clientId, reponse);
	}
	break;
	case ServerCommand::ExitRoom:
	{
		string room;
		string pseudo = "";
		_packet >> room;
		//recher utilisateur
		unique_lock <mutex> lRoom(mRoom);
		int roomid = FindRoomId(room);
		bool presence = roomid >= 0;
		if (presence)pseudo = listeRoom[roomid].findPseudoWithSocket(_clientId);
		presence = pseudo != "";

		//envoie réponse
		if (presence) reponse << true;
		else reponse << false;
		//tcp.Send(_clientId, reponse);

		//process
		if (presence) listeRoom[roomid].removeUser(pseudo);
	}
	break;
	case ServerCommand::joinRoom:
	{
		string roomName;
		unsigned short Roomid = 0;
		bool presence = false;
		_packet >> roomName;

		user new_user;
		_packet >> new_user.pseudo;
		new_user.id = _clientId;
		//recherche de la salle
		unique_lock <mutex> lRoom(mRoom);
		Roomid = FindRoomId(roomName);
		presence = Roomid >= 0;
		//test replicat
		bool replicat = false;
		if (presence) replicat = listeRoom[Roomid].testReplicatAdresse(_clientId);
		//réponse
		bool proceed = presence && !replicat;
		if (proceed)reponse << true;
		else reponse << false;
		//tcp.Send(_clientId, reponse);
		//ajout si possible
		if (proceed) listeRoom[Roomid].addUser(new_user);
		lRoom.unlock();
	}
	break;
	case ServerCommand::print:
	{
		string message;
		_packet >> message;
		cout << message;
	}
	reponse << true;
	//tcp.Send(_clientId, reponse);
	break;
	case ServerCommand::serverInfo:
	{
		_packet >> myAddress;
		_packet >> myPort;
	}
	break;
	default:
		reponse << false;
		//tcp.Send(_clientId, reponse);
		break;
	}

}

void Server::print()
{
	unique_lock<mutex> lListener(mListener);
	unique_lock<mutex> lRoom(mRoom);
	cout << "listeConnection" << endl;
	//for (int i = 0; i < listeConnection.size(); i++)cout << listeConnection[i]->socket.getRemoteAddress() << ":" << listeConnection[i]->socket.getRemotePort() << "   " << listeConnection[i]->state << endl;
	cout << "listeRoom:" << endl;
	for (int i = 0; i < listeRoom.size(); i++)listeRoom[i].Room_server::printS();
}