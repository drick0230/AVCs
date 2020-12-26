#include "room.h"

#pragma region ROOM

Room::Room(string _name) : name(_name) {};

void Room::print()
{
	cout << name << endl;
	for (int i = 0; i < listeUser.size(); i++)
	{
		cout << listeUser[i].pseudo << '\t' << listeUser[i].id << endl;
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
		if (new_user.id == listeUser[i].id)
		{
			throw string("replicat adresse  " + new_user.id);
		}
	}
	listeUser.push_back(new_user);
}

void Room::addUser(string user_pseudo, unsigned int _id)
{
	user new_user;
	new_user.pseudo = user_pseudo;
	new_user.id = _id;
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
		try { Room::addUser(userTry); }
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

bool Room::addUserAdaptative(string user_pseudo, unsigned int _id)
{
	user new_user;
	new_user.pseudo = user_pseudo;
	new_user.id = _id;
	return Room::addUserAdaptative(new_user);
}

void Room::removeUser(string user_pseudo)
{
	bool find = false;
	for (unsigned short i = 0, j = listeUser.size(); i < j; i++)
	{
		if (listeUser[i].pseudo == user_pseudo)
		{
			listeUser[i] = listeUser[j - 1];
			listeUser.pop_back();
			find = true;
			break;
		}
	}
	if (!find) throw "pseudo inconnu";
}

#pragma endregion

#pragma region Serveur
Room_server::Room_server(string _name) :Room(_name), pTCP(NULL) {}
void Room_server::addUser(user new_user)
{
	//envoie des user existant
	for (int i = 0; i < listeUser.size(); i++)
	{
		unsigned short _userPort;
		std::string _userIpAddress = pTCP->GetClientInfo(_userPort, listeUser[i].id);

		Packet packet;
		packet << ClientCommand::addUser << name << listeUser[i].pseudo << _userIpAddress << _userPort;
		pTCP->Send(new_user.id, packet);
	}

	//ajout du nouvel utilisateur
	if (!addUserAdaptative(new_user)) throw string("probleme ajout user");
	string pseudo = listeUser[listeUser.size() - 1].pseudo;
	unsigned short _newUserPort;
	std::string _newUserIpAddress = pTCP->GetClientInfo(_newUserPort, new_user.id);

	Packet usernamePacket;
	usernamePacket << ClientCommand::username << name << pseudo << _newUserIpAddress << _newUserPort;
	pTCP->Send(new_user.id, usernamePacket);

	//envoie du nouvel utilisateur
	Packet addUserPacket;
	addUserPacket << ClientCommand::addUser << name << pseudo << _newUserIpAddress << _newUserPort;
	for (int i = 0; i < listeUser.size(); i++)
	{
		pTCP->Send(listeUser[i].id, addUserPacket);
	}
}

void Room_server::printS() { Room::print(); }

bool Room_server::testReplicatAdresse(unsigned int _id)
{
	for (int i = 0; i < listeUser.size(); i++)
	{
		if (listeUser[i].id == _id)return true;
	}
	return false;
}

string Room_server::findPseudoWithSocket(unsigned int _id)
{
	for (int i = 0; i < listeUser.size(); i++)
	{
		if (listeUser[i].id == _id) return listeUser[i].pseudo;
	}
	return string("");
}

void Room_server::removeUser(string pseudo_user)
{
	for (int i = 0; i < listeUser.size(); i++)
	{
		Packet packet;
		packet << ClientCommand::removeUser << name << pseudo_user;
		pTCP->Send(listeUser[i].id, packet);
	}
	Room::removeUser(pseudo_user);
}

#pragma endregion

#pragma region client
Room_client::Room_client(string _name) : udp(), tListen(), hasIdentity(false), endReception(false), pseudo(""), Room(_name), tReception(&Room_client::fReception, this) {}
void Room_client::print()
{
	cout << "username: " << pseudo << endl;
	Room::print();
}

void Room_client::send(Packet _packet)
{
	unique_lock<mutex> lsocket(msocket);
	for (int i = 0; i < listeUser.size(); i++)
	{
		if (listeUser[i].pseudo != pseudo)
		{
			udp.Send(listeUser[i].id, _packet);
		}
	}
}

void Room_client::fReception()
{
	while (true)
	{
		Packet _packet;
		unsigned int _clientID = udp.WaitReceive(_packet);

		std::string _recvMsg = "";

		_packet >> _recvMsg;
		std::cout << "U received a msg :\n"
			      << _recvMsg << '\n';
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

void Room_client::setIdentity(std::string _ipAddress, unsigned short _port)
{
	if (udp.Bind(_ipAddress, _port))
	{
		tListen = std::thread(&Room_client::fReception, this);
		tListen.detach();
	}
	else throw "Bind ERROR";
	hasIdentity = true;
}
#pragma endregion