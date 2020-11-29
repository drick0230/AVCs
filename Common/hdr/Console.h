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

struct Text {
	struct Vector2_uint pos;
	struct Vector2_uint size;
	std::string content;
};

class Console {
private:
	// Variables
	HANDLE hOut;
	HANDLE hIn;
	INPUT_RECORD inRecord[30];
public:
	// Variables
	Vector2_uint pos;
	Vector2_uint size;

	Text inText;
	Text outText;
	std::string inKeys;


	// Constructors
	Console(Vector2_uint _pos = { 0, 0 }, Vector2_uint _size = { 0, 0 }, Text _inText = { {0,0}, {0,0}, "" }, Text _outText = { {0,0}, {0,0}, "" });

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

	void UpdateInText();
	void UpdateOutText();

	void Show();
	void Hide();
};
