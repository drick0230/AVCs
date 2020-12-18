#include "Console.h"

Vector2_int::Vector2_int(int _x, int _y) : x(_x), y(_y) {}
Vector2_uint::Vector2_uint(unsigned int _x, unsigned int _y) : x(_x), y(_y) {}
Color::Color(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b) {}

///////CONSOLE/////////
#pragma region Console
// Static Members
HANDLE Console::hOut;
HANDLE Console::hIn;
INPUT_RECORD Console::inRecord[30];
std::vector<unsigned short> Console::inKeys;

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
void Console::SetFontColor(Color _color) { SetFontColor(_color.r, _color.g, _color.b); }
void Console::DefaultFontColor() { printf_s("%c[39m", '\x1B'); }

void Console::SetBackgroundColor(unsigned char _r, unsigned char _g, unsigned char _b) { printf_s("%c[48;2;%d;%d;%dm", '\x1B', _r, _g, _b); }
void Console::SetBackgroundColor(Color _color) { SetBackgroundColor(_color.r, _color.g, _color.b); }
void Console::DefaultBackgroundColor() { printf_s("%c[49m", '\x1B'); }

void Console::SetScreenColor(unsigned char _r, unsigned char _g, unsigned char _b) { printf_s("%c]4;0;rgb:%x/%x/%x%c", '\x1B', _r, _g, _b, '\x1B'); }
#pragma endregion



// Public Function
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
	unsigned int _nbKeyEvent = 0;

	KEY_EVENT_RECORD _keyEvents[3];

	GetNumberOfConsoleInputEvents(hIn, &_nbEventAvailale);
	ReadConsoleInput(hIn, inRecord, _nbEventAvailale, &_nbEventRead);

	for (unsigned int _i = 0; _i < _nbEventRead; _i++)
	{
		if (inRecord[_i].EventType == KEY_EVENT) {
			if (inRecord[_i].Event.KeyEvent.bKeyDown)
			{
				_keyEvents[_nbKeyEvent] = inRecord[_i].Event.KeyEvent;
				_nbKeyEvent++;
			}
		}
	}

	switch (_nbKeyEvent)
	{
	case 3:
		if (_keyEvents[0].uChar.AsciiChar == 0x1b) // ESC
		{
			if (_keyEvents[1].uChar.AsciiChar == '[') // ESC[
			{
				if (_keyEvents[2].uChar.AsciiChar == 'A') // ESC[A
					inKeys.push_back(KEYS::U_ARROW);
				if (_keyEvents[2].uChar.AsciiChar == 'B') // ESC[B
					inKeys.push_back(KEYS::D_ARROW);
				if (_keyEvents[2].uChar.AsciiChar == 'C') // ESC[C
					inKeys.push_back(KEYS::R_ARROW);
				if (_keyEvents[2].uChar.AsciiChar == 'D') // ESC[D
					inKeys.push_back(KEYS::L_ARROW);
				if (_keyEvents[2].uChar.AsciiChar == 'H') // ESC[H
					inKeys.push_back(KEYS::HOME);
				if (_keyEvents[2].uChar.AsciiChar == 'F') // ESC[F
					inKeys.push_back(KEYS::END);
			}
		}
		break;
	case 2:

		break;
	case 1:
			inKeys.push_back(_keyEvents[0].uChar.AsciiChar);
		break;
	default:
		break;
	} 

	//for (unsigned int _i = 0; _i < _nbEventRead; _i++)
	//	if (inRecord[_i].EventType == KEY_EVENT)
	//		if (inRecord[_i].Event.KeyEvent.bKeyDown)
	//		{
	//			char _input = inRecord[_i].Event.KeyEvent.uChar.AsciiChar;
	//			// Si le caractère est imprimable
	//			if (_input >= (char)32 && _input <= (char)126) {
	//				// L'inclure dans le text
	//				inText.content.push_back(inRecord[_i].Event.KeyEvent.uChar.AsciiChar);
	//			}
	//			else {
	//				// L'inclure dans les touches de clavier
	//				inKeys.push_back(inRecord[_i].Event.KeyEvent.uChar.AsciiChar);
	//			}
	//		}

	if (_nbEventRead != 0)
		return true;
	return false;
}

// Return inKeys[_index] and remove it from the vector
unsigned short Console::GetInKeys(size_t _index) {
	unsigned short _return = inKeys[_index]; // Stocker la valeur à retourner
	inKeys.erase(inKeys.begin() + _index); // Retirer la valeur à retourner
	//inKeys.erase(_index, (size_t)_index + 1); // Effacer du string la valeur à retourner
	return _return;
}
#pragma endregion