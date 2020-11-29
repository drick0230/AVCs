﻿// AVCs_5.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets.

#pragma once

#include <SFML/Audio.hpp>
#include <thread>			// std::this_thread::sleep_for
#include <chrono>			// std::chrono::seconds
#include "VOIP.h"
#include "Console.h"

void ConsoleIO(Console& console, bool*, bool*, bool*, bool*, VOIP*);

template <class T>
T myParse(std::string);

// Interface
void MainMenu(Console& _console);

// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.
