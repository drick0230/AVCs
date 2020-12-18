// AVCs_5.cpp : définit le point d'entrée de l'application.
//
#include "AVCs_server.h"

int main()
{
	string nom = "cette salle à un nom";
	Room salle1(nom);
	salle1.addUserAdaptative("BEN", sf::IpAddress("1.123.152.230"), 120);
	salle1.addUserAdaptative("ALEx", sf::IpAddress("1.123.152.230"), 122);
	salle1.addUserAdaptative("ALEx", sf::IpAddress("1.123.152.230"), 123);
	salle1.addUserAdaptative("ALEx", sf::IpAddress("1.123.152.231"), 123);
	salle1.addUserAdaptative("ALEx", sf::IpAddress("1.123.152.232"), 123);
	salle1.addUserAdaptative("ALEx", sf::IpAddress("1.123.152.230"), 123);
	salle1.print();

	salle1.removeUser("ALEx0");
	salle1.removeUser("ALEx");
	salle1.removeUser("ALEx7");
	salle1.print();

	return 0;
}
