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

class Text {
private:
public:
	// Variables
	Vector2_int pos;
	Vector2_int size;
	std::string content;

	Console* console;

	// Constructors
	Text(Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(0, 0), std::string _content = "");
	Text(Console* _console, Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(0, 0), std::string _content = "");

	// Destructors
	~Text();

	// Functions
	void Show();
	void Hide();
};

class Console {
private:
	// Variables
	static HANDLE hOut;
	static HANDLE hIn;
	static INPUT_RECORD inRecord[30];
public:
	// Variables

	Vector2_int pos;
	Vector2_int size;

	Text inText;
	Text outText;
	static std::vector<unsigned short> inKeys;
	//std::string inKeys;


	// Constructors
	Console(Vector2_int _pos = { 0, 0 }, Vector2_int _size = { 0, 0 });
	Console(Vector2_int _pos, Vector2_int _size, Text _inText, Text _outText = { {0,0}, {0,0}, "" });

	// Destructors
	~Console();

	// Functions
	void Update();

#pragma region PrivateRealConsoleFunction
	static void GoTo(short, short);
	static void Move(short, short);
	static void EraseChar(unsigned int _n = 1);
	static void DeleteLine(unsigned int _n = 1);

	static void SetFontColor(unsigned char, unsigned char, unsigned char);
	static void SetScreenColor(unsigned char _r, unsigned char _g, unsigned char _b);
#pragma endregion

	static bool InitializeConsole();

	static bool Read();
	char GetInText(size_t _index);
	static unsigned short GetInKeys(size_t _index);

	void Show();
	void Hide();
};
