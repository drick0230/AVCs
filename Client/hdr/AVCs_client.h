﻿// AVCs_5.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets.

#pragma once

//#include <SFML/Audio.hpp>
#include <thread>			// std::this_thread::sleep_for
#include <chrono>			// std::chrono::seconds

//#include "VOIP.h"
#include "ATHElement.h"
#include "DevicesManager.h"
#include "Network.h"
#include "general.h"


//void ConsoleIO(bool*, bool*, bool*, bool*, VOIP*);


void serverTCP();
void clientTCP();

void serverUDP();
void clientUDP();

// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.
