#pragma once

/*#if      |Identifier|
  #elif     __linux__       Defined on Linux
  #endif    __sun           Defined on Solaris
            __FreeBSD__     Defined on FreeBSD
            __NetBSD__      Defined on NetBSD
            __OpenBSD__     Defined on OpenBSD
            __APPLE__       Defined on Mac OS X
            __hpux          Defined on HP-UX
            __osf__         Defined on Tru64 UNIX (formerly DEC OSF1)
            __sgi           Defined on Irix
            _AIX            Defined on AIX
            _WIN32          Defined on Windows
*/

#if _WIN32
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#endif


class Console {
private:

public:
    // Variables
    // Screen
    unsigned int xSize, ySize;
    //wchar_t* screen;
    HANDLE hOut;
    HANDLE hIn;
    INPUT_RECORD inputs[128];
    DWORD nbInputs;
    //DWORD bytesWritted;

	// Constructors
	Console();

	// Destructors
	~Console();

	// Functions
    void Update();

    bool InitializeConsole();
    void TestVirtualTerminal();

    void GoTo(unsigned int, unsigned int);
    void Read();
};
