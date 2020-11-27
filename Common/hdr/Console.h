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

struct Vector2_uint {
    unsigned int x;
    unsigned int y;
};

struct Screen {
    Vector2_uint size;
    char* buffer;
};

class Console {
private:

public:
    // Variables

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

    #pragma region PrivateRealConsoleFunction
        void ConsoleGoTo(short, short);
        void ConsoleGoTo(short, short, std::string &);
        void ConsoleEraseChar(unsigned int _n = 1);
        void ConsoleDeleteLine(unsigned int _n = 1);

        void ConsoleSetFontColor(unsigned char, unsigned char, unsigned char);
        void ConsoleSetScreenColor(unsigned char _r, unsigned char _g, unsigned char _b);
    #pragma endregion

    bool InitializeConsole();

    void Read();

    std::string ToHex(char);
};
