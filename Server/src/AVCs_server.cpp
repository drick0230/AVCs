// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_server.h"


int main()
{
	vector <Client*> listeClient;
	unsigned short port;
	cout << "port: " << endl;
	cin >> port;
	//Creation des thread
	Server server(port);

	//boucle de commande
	string commande = "";
	while (commande != "exit")
	{
		cout << "commande: " << endl;
		cin >> commande;
		if (commande == "print")
		{
			server.print();
		}
		if (commande == "openClient")
		{
			unsigned short port_server;
			string ipInput;
			cout << "Ip du server" << endl;
			cin >> ipInput;
			cout << "port du server" << endl;
			cin >> port_server;
			sf::IpAddress ip(ipInput);
			listeClient.push_back(new Client(ip, port_server));
			cout << "idClient = " << listeClient.size() - 1 << endl;
		}
		if (commande == "closeClient")
		{
			unsigned short id;
			cout << "idClient" << endl;
			cin >> id;
			if (listeClient[id])delete listeClient[id];
		}
		if (commande == "createRoomClient")
		{
			unsigned short id;
			string roomName;
			cout << "idClient" << endl;
			cin >> id;
			cout << "RoomName" << endl;
			cin >> roomName;
			listeClient[id]->createRoom(roomName);
		}
		if (commande == "joinRoomClient")
		{
			unsigned short id;
			string roomName;
			string pseudo;
			cout << "idClient" << endl;
			cin >> id;
			cout << "RoomName" << endl;
			cin >> roomName;
			cout << "Pseudo" << endl;
			cin >> pseudo;
			listeClient[id]->joinRoom(roomName,pseudo);
		}
		if (commande == "printClient")
		{
			unsigned short id;
			cout << "idClient" << endl;
			cin >> id;
			listeClient[id]->print();
		}
	}
	
	for (int i = 0; i < listeClient.size(); i++)if (listeClient[i])delete listeClient[i];
	listeClient.clear();

	return 0;
}
