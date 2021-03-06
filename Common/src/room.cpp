#include "room.h"

#pragma region ROOM

Room::Room(string _name) : name(_name) {};

void Room::print()
{
	cout << name << endl;
	for (int i = 0; i < listeUser.size(); i++)
	{
		cout << listeUser[i].pseudo << '\t' << listeUser[i].ip << ':' << listeUser[i].port << endl;
	}
}

void Room::addUser(user new_user)
{
	for (int i = 0; i < listeUser.size(); i++)
	{
		if (new_user.pseudo == listeUser[i].pseudo)
		{
			throw string("replicat pseudo");
		}
		if (new_user.ip == listeUser[i].ip && new_user.port == listeUser[i].port)
		{
			throw string("replicat adresse  " + new_user.ip.toString() + ":" + to_string(new_user.port));
		}
	}
	listeUser.push_back(new_user);
}

void Room::addUser(string user_pseudo, sf::IpAddress user_Ip, unsigned short user_port)
{
	user new_user;
	new_user.pseudo = user_pseudo;
	new_user.ip = user_Ip;
	new_user.port = user_port;
	Room::addUser(new_user);
}

bool Room::addUserAdaptative(user new_user)
{
	int adaptNum = -1;
	user userTry = new_user;
	bool finish = false;
	while (!finish)
	{
		finish = true;
		try { addUser(userTry); }
		catch (string msg)
		{
			if (msg == "replicat pseudo")
			{
				finish = false;
				adaptNum++;
				userTry.pseudo = new_user.pseudo + to_string(adaptNum);
			}
			else
			{
				cerr << msg << endl;
				return false;
			}
		}
	}
	return true;
}

bool Room::addUserAdaptative(string user_pseudo, sf::IpAddress user_Ip, unsigned short user_port)
{
	user new_user;
	new_user.pseudo = user_pseudo;
	new_user.ip = user_Ip;
	new_user.port = user_port;
	return Room::addUserAdaptative(new_user);
}

void Room::removeUser(string user_pseudo)
{
	bool find = false;
	for (unsigned short i = 0, j=listeUser.size() ; i < j; i++)
	{
		if (listeUser[i].pseudo == user_pseudo)
		{
			listeUser[i] = listeUser[j-1];
			listeUser.pop_back();
			find = true;
			break;
		}
	}
	if (!find) throw "pseudo inconnu";
}

#pragma endregion

#pragma region Serveur
Room_server::Room_server(string _name) :Room(_name){}
void Room_server::addUser(user new_user, sf::TcpSocket* socketPtr)
{
	//envoie des user existant
	for (int i = 0; i < listeUser.size(); i++)
	{
		socketPtr->setBlocking(true);
		sf::Packet packet;
		packet << ClientCommand::addUser << name << listeUser[i].pseudo << listeUser[i].ip.toInteger() << listeUser[i].port;
		socketPtr->send(packet);
	}

	//ajout du nouvel utilisateur
	if(!addUserAdaptative(new_user)) throw string("probleme ajout user");
	string pseudo = listeUser[listeUser.size() - 1].pseudo;

	sf::Packet usernamePacket;
	usernamePacket << ClientCommand::username << name << pseudo<<new_user.ip.toInteger()<<new_user.port;
	socketPtr->send(usernamePacket);

	userSocket newSocket;
	newSocket.pseudo = pseudo;
	newSocket.socketPtr = socketPtr;
	listeSocket.push_back(newSocket);

	//envoie du nouvel utilisateur
	for (int i = 0; i < listeUser.size(); i++)
	{
		listeSocket[i].socketPtr->setBlocking(true);
		sf::Packet packet;
		packet << ClientCommand::addUser << name << pseudo << new_user.ip.toInteger() << new_user.port;
		listeSocket[i].socketPtr->send(packet);
	}

}

void Room_server::printS() { Room::print(); }

bool Room_server::testReplicatAdresse(sf::IpAddress ip, unsigned short port)
{
	for (int i = 0; i < listeUser.size(); i++)
	{
		if (listeUser[i].ip == ip && listeUser[i].port == port)return true;
	}
	return false;
}

string Room_server::findPseudoWithSocket(sf::TcpSocket* socketPtr)
{
	for (int i = 0; i < listeSocket.size(); i++)
	{
		if (listeSocket[i].socketPtr == socketPtr) return listeSocket[i].pseudo;
	}
	return string("");
}

void Room_server::removeUser(string pseudo_user)
{
	for (int i = 0; i < listeSocket.size(); i++)
	{
		sf::Packet packet;
		packet<<ClientCommand::removeUser << name << pseudo_user;
		listeSocket[i].socketPtr->setBlocking(true);
		listeSocket[i].socketPtr->send(packet);
	}
	Room::removeUser(pseudo_user);
}

#pragma endregion

#pragma region client
Room_client::Room_client(string _name) :socket(),hasIdentity(false),endReception(false), pseudo(""),Room(_name),tReception(&Room_client::fReception,this) {}
void Room_client::print()
{
	cout << "username: " << pseudo << endl;
	Room::print();
}

void Room_client::send(sf::Packet packet)
{
	unique_lock<mutex> lsocket(msocket);
	socket.setBlocking(true);
	for (int i = 0; i < listeUser.size(); i++)
	{
		if (listeUser[i].pseudo != pseudo)
		{
			socket.send(packet, listeUser[i].ip, listeUser[i].port);
		}
	}
}

void Room_client::fReception()
{
	while (true)
	{
		unique_lock<mutex> lsocket(msocket);
		if (endReception)break;
		if (hasIdentity) {
			socket.setBlocking(false);

			for (int i = 0; i < listeUser.size(); i++)
			{
				sf::Socket::Status state(sf::Socket::Status::Partial);
				sf::Packet packet;
				while (state == sf::Socket::Status::Partial) state = socket.receive(packet, listeUser[i].ip, listeUser[i].port);
				if (state == sf::Socket::Status::Done)
				{
					string message;
					packet >> message;
					cout << pseudo << "<-" << listeUser[i].pseudo << " : " << message << endl;
				}
			}

			lsocket.unlock();
		}
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

Room_client::~Room_client()
{
	//Libération des thread
	unique_lock<mutex> lSocket(msocket);
	endReception = true;
	lSocket.unlock();
	if (tReception.joinable())tReception.join();
}

void Room_client::setIdentity(user identity)
{
	//unique_lock<mutex> lSocket(msocket);
	socket.setBlocking(true);
	hasIdentity = true;
}
#pragma endregion