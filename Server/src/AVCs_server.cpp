// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_server.h"

int main()
{
	string nom = "cette salle à un nom";
	Room salle1(nom);
	vector <string> commande = split("allo  comment ca   va", ' ',true);
	cout << commande.size()<<endl;
	for (int i = 0; i < commande.size(); i++) cout << commande[i] << endl;

	return 0;
}
