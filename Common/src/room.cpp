#include "server.h"
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
Room_server::Room_server(string _name, void* _parent, UDP* _pUDP) :Room(_name), pUDP(_pUDP), parent(_parent) { }
void Room_server::addUser(user new_user)
{
	//ajout du nouvel utilisateur
	if (!addUserAdaptative(new_user)) throw string("probleme ajout user");
	string pseudo = listeUser[listeUser.size() - 1].pseudo;

	unsigned short _newUserPort = pUDP->portBook[new_user.id];
	std::string _newUserIpAddress = pUDP->addressBook[new_user.id];

	// Is-he on the same network as me?
	if (_newUserIpAddress == ((Server*)parent)->defaultGateway)
		_newUserIpAddress = ((Server*)parent)->publicIP; // Return the public address of our Network

	Packet usernamePacket;
	usernamePacket << ClientCommand::username << name << pseudo;
	pUDP->Send(new_user.id, usernamePacket);

	//envoie des user au nouvel utilisateur
	for (int i = 0; i < listeUser.size() - 1; i++)
	{
		unsigned short _userPort = pUDP->portBook[listeUser[i].id];
		std::string _userIpAddress = pUDP->addressBook[listeUser[i].id];

		Packet packet;
		packet << ClientCommand::addUser << name << listeUser[i].pseudo << _userIpAddress << _userPort;
		pUDP->Send(new_user.id, packet);
	}

	//envoie du nouvel utilisateur aux autres utilisateurs
	Packet addUserPacket;
	addUserPacket << ClientCommand::addUser << name << pseudo << _newUserIpAddress << _newUserPort;
	for (int i = 0; i < listeUser.size(); i++)
	{
		if (listeUser[i].id != new_user.id)
			pUDP->Send(listeUser[i].id, addUserPacket);
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
		pUDP->Send(listeUser[i].id, packet);
	}
	Room::removeUser(pseudo_user);
}

#pragma endregion

#pragma region client
Room_client::Room_client(string _name) : udp(), hasIdentity(false), endReception(false), pseudo(""), Room(_name), tReception() {}
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

void Room_client::setIdentity(std::string _pseudo)
{
	//_ipAddress = "192.168.1.141";

	pseudo = _pseudo;
	if (udp.Bind(INADDR_ANY, 0))
	{
		tReception = std::thread(&Room_client::fReception, this);
		tReception.detach();
	}
	else throw "Bind ERROR";
	hasIdentity = true;
}

void Room_client::KeepAlive(unsigned int _clientID, unsigned int _ms) {
	while (1) {
		Packet _packet;
		_packet << std::string("KEEP_ALIVE");
		udp.Send(_clientID, _packet);
		std::this_thread::sleep_for(std::chrono::milliseconds(_ms));
	}
}
#pragma endregion