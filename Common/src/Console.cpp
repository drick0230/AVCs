#include "Console.h"

#ifdef _WIN32

// Constructors
Console::Console(Vector2_uint _pos, Vector2_uint _size, Text _inText, Text _outText) : hOut(), hIn()
{
	pos = _pos;
	size = _size;

	inText = _inText;
	inKeys = "";

	outText = _outText;

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
void Console::GoTo(short _x, short _y) { printf("%c[%d;%dH", '\x1B', _y + 1, _x + 1); }
void Console::Move(short _x, short _y) {
	if (_x > 0) 
		printf("%c[%dC", '\x1B', _x); // Move Frontward by _x
	else if (_x < 0)
		printf("%c[%dD", '\x1B', _x); // Move Backward by _x

	if (_y > 0)
		printf("%c[%dA", '\x1B', _y); // Move Up by _y
	else if (_y < 0)
		printf("%c[%dB", '\x1B', _y); // Move Down by _y
}

void Console::EraseChar(unsigned int _n) { printf("%c[%dX", '\x1B', _n); }

void Console::DeleteLine(unsigned int _n) { printf("%c[%dM", '\x1B', _n); }

void Console::SetFontColor(unsigned char _r, unsigned char _g, unsigned char _b) { printf("%c[38;2;%d;%d;%dm", '\x1B', _r, _g, _b); }

void Console::SetScreenColor(unsigned char _r, unsigned char _g, unsigned char _b) { printf("%c]4;0;rgb:%x/%x/%x%c", '\x1B', _r, _g, _b, '\x1B'); }
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
				// Si le caract�re est imprimable
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
	char _return = inText.content[_index]; // Stocker la valeur � retourner
	inText.content.erase(_index, (size_t)_index+1); // Effacer du string la valeur � retourner
	return _return;
}
char Console::GetInKeys(size_t _index) {
	char _return = inKeys[_index]; // Stocker la valeur � retourner
	inKeys.erase(_index, (size_t)_index + 1); // Effacer du string la valeur � retourner
	return _return;
}

// Met � jour l'affichage du texte entrant sur la console
void Console::UpdateInText() {
	GoTo(inText.pos.x, inText.pos.y);
	EraseChar(inText.size.x);
	std::cout << inText.content;
}

// Met � jour l'affichage du texte sortant sur la console
void Console::UpdateOutText() {
	GoTo(outText.pos.x, outText.pos.y);
	EraseChar(outText.size.x);
	std::cout << outText.content;
}

// Affiche la console
void Console::Show() {

	SetFontColor(255, 255, 255);
	SetScreenColor(20, 20, 20);
	GoTo(pos.x, pos.y);

	// Top Line
	for (unsigned char _i = 0; _i < size.x; _i++)
		printf("%c", (char)219);

	std::string _commands = "";
	// Side Border
	for (int _i = 1; _i < size.y; _i++) {
		GoTo(pos.x, pos.y + _i);
		printf("%c", (char)219);
		EraseChar(size.x);
		GoTo(pos.x + size.x - 1, pos.y + _i);
		printf("%c", (char)219);
	}
	//std::cout << _commands;

	// Mid Line
	GoTo(pos.x, pos.y + size.y - inText.size.y - 1);
	for (unsigned char _i = 0; _i < size.x - 1; _i++)
		printf("%c", (char)219);

	// Bot Line
	GoTo(pos.x, pos.y + size.y);
	for (unsigned char _i = 0; _i < size.x; _i++)
		printf("%c", (char)219);
}

// Cache la console
void Console::Hide() {
	GoTo(pos.x, pos.y);

	// Erase Top Line
	EraseChar(size.x);

	std::string _commands = "";
	// Erase Side Border
	for (int _i = 1; _i < size.y; _i++) {
		GoTo(pos.x, pos.y + _i);
		EraseChar(size.x);
	}

	// Erase Mid Line
	GoTo(pos.x, pos.y + size.y - inText.size.y - 1);
	EraseChar(size.x - 1);

	// Bot Line
	GoTo(pos.x, pos.y + size.y);
	EraseChar(size.x);
}

#endif