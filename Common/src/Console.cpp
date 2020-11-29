#include "Console.h"

// Constructors
Console::Console(unsigned int _sizeX, unsigned int _sizeY, unsigned int _inTextPosX, unsigned int _inTextPosY, unsigned int _inTextSizeX, unsigned int _inTextSizeY, unsigned int _outTextPosX, unsigned int _outTextPosY, unsigned int _outTextSizeX, unsigned int _outTextSizeY) : hOut(), hIn() {
	inText.pos = { _inTextPosX , _inTextPosY };
	inText.size = { _inTextSizeX , _inTextSizeY };
	inText.content = "";
	inKeys = "";

	outText.pos = { _outTextPosX , _outTextPosY };
	outText.size = { _outTextSizeX , _outTextSizeY };
	outText.content = "";

	for (unsigned char _i = 0; _i < 30; _i++)
		inRecord[_i] = INPUT_RECORD();

	if (!InitializeConsole())
		std::cout << "Failed to initialize Console as Virtual Terminal Sequences";
}

// Destructors
Console::~Console() {

}

// Private Functions
#pragma region PrivateRealConsoleFunction
void Console::GoTo(short _x, short _y) { printf_s("%c[%d;%dH", '\x1B', _y + 1, _x + 1); }
void Console::Move(short _x, short _y) {
	if (_x > 0) 
		printf_s("%c[%dC", '\x1B', _x); // Move Frontward by _x
	else if (_x < 0)
		printf_s("%c[%dD", '\x1B', _x); // Move Backward by _x

	if (_y > 0)
		printf_s("%c[%dA", '\x1B', _y); // Move Up by _y
	else if (_y < 0)
		printf_s("%c[%dB", '\x1B', _y); // Move Down by _y
}

void Console::EraseChar(unsigned int _n) { printf_s("%c[%dX", '\x1B', _n); }

void Console::DeleteLine(unsigned int _n) { printf_s("%c[%dM", '\x1B', _n); }

void Console::SetFontColor(unsigned char _r, unsigned char _g, unsigned char _b) { printf_s("%c[38;2;%d;%d;%dm", '\x1B', _r, _g, _b); }

void Console::SetScreenColor(unsigned char _r, unsigned char _g, unsigned char _b) { printf_s("%c]4;0;rgb:%x/%x/%x%c", '\x1B', _r, _g, _b, '\x1B'); }
#pragma endregion



// Public Function
void Console::Update() {
	GoTo(0, 0);
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

// Read the next inputs (Non Blocking)
bool Console::Read() {
	unsigned long _nbEventAvailale = 0;
	unsigned long _nbEventRead = 0;

	GetNumberOfConsoleInputEvents(hIn, &_nbEventAvailale);
	ReadConsoleInput(hIn, inRecord, _nbEventAvailale, &_nbEventRead);

	for (unsigned int _i = 0; _i < _nbEventRead; _i++)
		if (inRecord[_i].EventType == KEY_EVENT)
			if (inRecord[_i].Event.KeyEvent.bKeyDown)
			{
				char _input = inRecord[_i].Event.KeyEvent.uChar.AsciiChar;
				// Si le caractère est imprimable
				if (_input >= (char)32 && _input <= (char)126) {
					// L'inclure dans le text
					inText.content.push_back(inRecord[_i].Event.KeyEvent.uChar.AsciiChar);
				}
				else {
					// L'inclure dans les touches de clavier
					inKeys.push_back(inRecord[_i].Event.KeyEvent.uChar.AsciiChar);
				}
			}

	if (_nbEventRead != 0)
		return true;
	return false;
}

// Return inText[_index] and remove it from the string
char Console::GetInText(size_t _index) {
	char _return = inText.content[_index]; // Stocker la valeur à retourner
	inText.content.erase(_index, (size_t)_index+1); // Effacer du string la valeur à retourner
	return _return;
}
char Console::GetInKeys(size_t _index) {
	char _return = inKeys[_index]; // Stocker la valeur à retourner
	inKeys.erase(_index, (size_t)_index + 1); // Effacer du string la valeur à retourner
	return _return;
}