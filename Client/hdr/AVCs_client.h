// AVCs_5.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets.

#pragma once

#include <SFML/Audio.hpp>
#include <thread>			// std::this_thread::sleep_for
#include <chrono>			// std::chrono::seconds
#include "VOIP.h"
#include "Console.h"
#include "AsciiImage.h"

#ifdef _WIN32
void ConsoleIO(Console& console, bool*, bool*, bool*, bool*, VOIP*);
// Interface
void MainMenu(Console& _console);
#endif

template <class T>
T myParse(std::string);
// TODO: Référencez ici les en-têtes supplémentaires nécessaires à votre programme.
