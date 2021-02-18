# Another Voice Chat software
## Description
L'objectif de ce logiciel est de permettre la communication vocale directe entre des pairs, de consommer peu de ressources système et d'être léger.\
Le langage c++ est performant, l'interface en ligne de commande requiert peu de la carte graphique et l'utilisation d'API 
système (Win32 et Linux kernel user-space API) permet d'éviter les dépendances à d'autres programmes.

Ce logiciel devrait être le premier d'une série de quatre logiciels. Ces quatre logiciels devraient permettre la
communication Audio, la communication Video (webcam), la messagerie (Chatting) et le partage d'écran (Screen sharing).

## Utilisation (version 0.5)
*	Seul l'exécutable "AVCs_client.exe" est utilisé.
*	L'exécutable est téléchargeable via la section "Releases", en haut à droite de la page.
*	L'exécutable est aussi disponible, après la compilation, dans "AVCs\out\build\x64-Release\Client" pour le "Release" et dans "AVCs\out\build\x64-Debug (par défaut)\Client" pour le "Debug".

### Client
1.	Inscrire <0> dans la console et appuyer sur Entrer
2.	Inscrire le numéro correspondant à son périphérique d'entrée audio et appuyer sur Entrer
3.	Inscrire le numéro correspondant à son périphérique de sortie audio et appuyer sur Entrer
4.	Inscrire le numéro correspondant à l'adresse IPV4 de son interface réseau et appuyer sur Entrer
5.	Inscrire l'adresse IPV4 du serveur de rendez-vous et appuyer sur Entrer

### Serveur
Le port utilisé par le serveur de l'application est <11111>.

1.	Inscrire <1> dans la console et appuyer sur Entrer
2.	Inscrire sa passerelle par défaut au format IPV4 et appuyer sur Entrer
3.	Inscrire l'adresse IPV4 public qui permettra aux clients de communiquer avec le serveur et appuyer sur Entrer

## Compilation
*	Le projet utilise CMake version 3.8.
*	L'IDE employé à l'origine du projet est Visual Studio 2019.
*	Le programme est actuellement compatible qu'avec Windows et son fonctionnement n'est pas assuré sur les versions antérieures à Windows 10.
*	La branche actuellement en développement et fonctionnelle est "AVCs_Version_0.5".

## API / Library / SDK
### [Win32 API](https://docs.microsoft.com/en-us/windows/win32/)
*	[Console Virtual Sequences](https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences)
*	[Windows Sockets 2](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2)
	*	Protocole UDP
*	[Microsoft Media Foundation](https://docs.microsoft.com/en-us/windows/win32/medfound/microsoft-media-foundation-sdk)
	*	[Source Reader](https://docs.microsoft.com/en-us/windows/win32/medfound/source-reader)
	*	[Sink Writter](https://docs.microsoft.com/en-us/windows/win32/medfound/sink-writer)
