#include "Console.h"

// Constructors
Console::Console() : hOut(), hIn(){
	// Screen Initialization
	xSize = 120;
	ySize = 40;

	if (!InitializeConsole())
		std::cout << "Failed to initialize Console as Virtual Terminal Sequences";
	//screen = new wchar_t[xSize * ySize];
	//hConsoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	//bytesWritted = 0;
}

// Destructors
Console::~Console() {

}

// Functions
void Console::Update(){

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

void Console::TestVirtualTerminal() {
	// Try some Set Graphics Rendition (SGR) terminal escape sequences
	wprintf(L"\x1b[31mThis text has a red foreground using SGR.31.\r\n");
	wprintf(L"\x1b[1mThis text has a bright (bold) red foreground using SGR.1 to affect the previous color setting.\r\n");
	wprintf(L"\x1b[mThis text has returned to default colors using SGR.0 implicitly.\r\n");
	wprintf(L"\x1b[34;46mThis text shows the foreground and background change at the same time.\r\n");
	wprintf(L"\x1b[0mThis text has returned to default colors using SGR.0 explicitly.\r\n");
	wprintf(L"\x1b[31;32;33;34;35;36;101;102;103;104;105;106;107mThis text attempts to apply many colors in the same command. Note the colors are applied from left to right so only the right-most option of foreground cyan (SGR.36) and background bright white (SGR.107) is effective.\r\n");
	wprintf(L"\x1b[39mThis text has restored the foreground color only.\r\n");
	wprintf(L"\x1b[49mThis text has restored the background color only.\r\n");
}

void Console::GoTo(unsigned int _x, unsigned int _y) {
	//wprintf(L"\x1B");
	std::cout << '\x1B' << "[" << _x+1 << ';' << _y+1 << "Htest 1;5";
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