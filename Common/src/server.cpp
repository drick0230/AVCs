#include "server.h"

Server::Server(unsigned short port) : endListener(false), tListener(&Server::fListener,this,port)
{
	
}

Server::~Server()
{
	//Libération des thread
	unique_lock<mutex> lListener(mListener);
	endListener = true;
	listener.close();
	lListener.unlock();
	if (tListener.joinable())tListener.join();
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

void Server::print()
{
	unique_lock<mutex> lListener(mListener);
	unique_lock<mutex> lRoom(mRoom);
	cout << "listeConnection" << endl;
	for (int i = 0; i < listeConnection.size(); i++)cout << listeConnection[i]->socket.getRemoteAddress() << ":" << listeConnection[i]->socket.getRemotePort() << "   " << listeConnection[i]->state << endl;
	cout << "listeRoom:" << endl;
	for (int i = 0; i < listeRoom.size(); i++)listeRoom[i].print();
}