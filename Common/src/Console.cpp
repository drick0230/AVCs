#include "Console.h"

// Constructors
Console::Console() : hOut(), hIn() {
	// Screen Initialization
	//screen.buffer = new char[(unsigned int)(screen.size.x * screen.size.y)];
	//screen.size.x = 120;
	//screen.size.y = 40;

	if (!InitializeConsole())
		std::cout << "Failed to initialize Console as Virtual Terminal Sequences";
}

// Destructors
Console::~Console() {

}

// Private Functions
#pragma region PrivateRealConsoleFunction
void Console::ConsoleGoTo(short _x, short _y) { 
	printf_s("%c[%d;%dH", '\x1B', _y+1, _x+1);
	//std::cout << '\x1B' << "[" << _y + 1 << ';' << _x + 1 << 'H';
}

void Console::ConsoleGoTo(short _x, short _y, std::string &_outs) {
	char _tString[50];
	sprintf_s(_tString, "%c[%d;%dH", '\x1B', _y, _x);
	_outs.append(_tString);
}

void Console::ConsoleEraseChar(unsigned int _n) { 
	printf_s( "%c[%dX", '\x1B', _n);
}

void Console::ConsoleDeleteLine(unsigned int _n) { std::cout << '\x1B' << "[" << _n << 'M'; }

void Console::ConsoleSetFontColor(UINT8 _r, UINT8 _g, UINT8 _b) { std::cout << '\x1B' << "[38;2;" << (short)_r << ';' << (short)_g << ';' << (short)_b << 'm'; }

void Console::ConsoleSetScreenColor(UINT8 _r, UINT8 _g, UINT8 _b) { std::cout << '\x1B' << "]4;0;rgb:" << std::hex << (short)_r << '/' << std::hex << (short)_g << '/' << std::hex << (short)_b << '\x1B'; }
#pragma endregion



// Public Function
void Console::Update() {
	ConsoleGoTo(0, 0);
	//for (unsigned int _i = 0; _i < screen.size.x * screen.size.y; _i++) {
	//	std::cout << screen.buffer[_i];
	//}
}

bool Console::InitializeConsole()
{
	// Set output mode to handle virtual terminal sequences
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	hIn = GetStdHandle(STD_INPUT_HANDLE);
	if (hIn == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD dwOriginalOutMode = 0;
	DWORD dwOriginalInMode = 0;
	if (!GetConsoleMode(hOut, &dwOriginalOutMode))
	{
		return false;
	}
	if (!GetConsoleMode(hIn, &dwOriginalInMode))
	{
		return false;
	}

	DWORD dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;

	DWORD dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
	if (!SetConsoleMode(hOut, dwOutMode))
	{
		// we failed to set both modes, try to step down mode gracefully.
		dwRequestedOutModes = ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		dwOutMode = dwOriginalOutMode | dwRequestedOutModes;
		if (!SetConsoleMode(hOut, dwOutMode))
		{
			// Failed to set any VT mode, can't do anything here.
			return false;
		}
	}

	DWORD dwInMode = dwOriginalInMode | ENABLE_VIRTUAL_TERMINAL_INPUT;
	if (!SetConsoleMode(hIn, dwInMode))
	{
		// Failed to set VT input mode, can't do anything here.
		return false;
	}

	return true;
}

// Read the next input (Blocking)
void Console::Read() {
	ReadConsoleInput(hIn, inputs, 128, &nbInputs);
	for (unsigned int i = 0; i < nbInputs; i++) {
		if (inputs[i].EventType == KEY_EVENT) {
			if (inputs[i].Event.KeyEvent.bKeyDown)
				std::cout << "Key Pressed\n";
			else
				std::cout << "Key Released\n";
		}
	}
}

std::string Console::ToHex(char _c) {
	char _hex[3];
	sprintf_s(_hex, "%x", _c);
	return _hex;
}