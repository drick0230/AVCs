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

// Class Prototype
class Console;

struct Vector2_uint {
	unsigned int x;
	unsigned int y;
};

struct Vector2_int {
	int x;
	int y;
};

class Text {
private:
public:
	// Variables
	struct Vector2_int pos;
	struct Vector2_int size;
	std::string content;

	Console* console;

	// Constructors
	Text(Vector2_int _pos = { 0, 0 }, Vector2_int _size = { 0, 0 }, std::string _content = "");
	Text(Console* _console, Vector2_int _pos = { 0, 0 }, Vector2_int _size = { 0, 0 }, std::string _content = "");

	// Destructors
	~Text();

	// Functions
	void Show();
	void Hide();
};

class Console {
private:
	// Variables
	HANDLE hOut;
	HANDLE hIn;
	INPUT_RECORD inRecord[30];
public:
	// Variables
	Vector2_int pos;
	Vector2_int size;

	Text inText;
	Text outText;
	std::string inKeys;


	// Constructors
	Console(Vector2_int _pos = { 0, 0 }, Vector2_int _size = { 0, 0 });
	Console(Vector2_int _pos, Vector2_int _size, Text _inText, Text _outText = { {0,0}, {0,0}, "" });

	// Destructors
	~Console();

	// Functions
	void Update();

#pragma region PrivateRealConsoleFunction
	void GoTo(short, short);
	void Move(short, short);
	void EraseChar(unsigned int _n = 1);
	void DeleteLine(unsigned int _n = 1);

	void SetFontColor(unsigned char, unsigned char, unsigned char);
	void SetScreenColor(unsigned char _r, unsigned char _g, unsigned char _b);
#pragma endregion

	bool InitializeConsole();

	bool Read();
	char GetInText(size_t _index);
	char GetInKeys(size_t _index);

	void Show();
	void Hide();
};
