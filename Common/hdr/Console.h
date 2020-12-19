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
#include <vector>
#endif

namespace KEYS {
	const enum KEYS { U_ARROW = 256, D_ARROW, L_ARROW, R_ARROW, HOME, END };
}

// Class Prototype
class Console;

class Vector2_uint {
public:
	unsigned int x;
	unsigned int y;
	Vector2_uint(unsigned int _x = 0, unsigned int _y = 0);
};

class Vector2_int {
public:
	int x;
	int y;
	Vector2_int(int _x = 0, int _y = 0);
};

class Color {
public:
	unsigned char r, g, b;
	Color(unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0);
};

class Console {
private:
	// Variables
	static HANDLE hOut;
	static HANDLE hIn;
	static INPUT_RECORD inRecord[30];

	static std::string outCommands; // Commands buffer to output in the console
public:
	// Variables
	static std::vector<unsigned short> inKeys;

	// Functions
#pragma region PrivateRealConsoleFunction
	static void GoTo(short, short);
	static void Move(short, short);
	static void EraseChar(unsigned int _n = 1);
	static void DeleteLine(unsigned int _n = 1);

	static void SetFontColor(unsigned char, unsigned char, unsigned char);
	static void SetFontColor(Color _color);
	static void DefaultFontColor();

	static void SetBackgroundColor(unsigned char, unsigned char, unsigned char);
	static void SetBackgroundColor(Color _color);
	static void DefaultBackgroundColor();

	static void SetScreenColor(unsigned char _r, unsigned char _g, unsigned char _b);
#pragma endregion

	static bool InitializeConsole();

	static bool Read();
	static unsigned short GetInKeys(size_t _index);

	static void Write(std::string _s);
	static void Write(char _c);
	static void Write(unsigned char _c, int _Radix = 10);
	static void Write(int _int, int _Radix = 10);

	static void Write(); // Apply the commands send to the output of the Console
};
