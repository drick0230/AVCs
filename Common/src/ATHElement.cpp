#include "ATHElement.h"

// Constructors
ATHElement::ATHElement(Vector2_int _pos, Vector2_int _size) {
	pos = _pos;
	size = _size;
}

VLine::VLine(Vector2_int _pos, int _size) : ATHElement(_pos) {
	size = _size;
}

HLine::HLine(Vector2_int _pos, int _size) : VLine(_pos, _size) {}

Rect::Rect(Vector2_int _pos, Vector2_int _size) : 
	ATHElement(_pos, _size), topLine(_pos, _size.x),
	leftLine(Vector2_int(_pos.x, pos.y + 1), _size.y - 1),
	rightLine(Vector2_int(_pos.x + size.x - 1, pos.y + 1), _size.y - 1),
	botLine(Vector2_int(_pos.x + 1, pos.y + size.y - 1), _size.x - 2) {}

// Functions
void VLine::Show() {

	for (int _i = 0; _i < size; _i++) {
		Console::GoTo(pos.x, pos.y + _i);
		printf_s("%c", (char)219);
	}
}

void VLine::Hide() {
	for (int _i = 0; _i < size; _i++) {
		Console::GoTo(pos.x, pos.y + _i);
		printf_s("%c", ' ');
	}
}

void HLine::Show() {
	Console::GoTo(pos.x, pos.y);
	for (unsigned char _i = 0; _i < size; _i++)
		printf_s("%c", (char)219);
}

void HLine::Hide() {
	Console::GoTo(pos.x, pos.y);
	for (unsigned char _i = 0; _i < size; _i++)
		printf_s("%c", ' ');
}

void Rect::Show() {
	topLine.Show();
	botLine.Show();
	rightLine.Show();
	leftLine.Show();
}

void Rect::Hide() {
	topLine.Hide();
	botLine.Hide();
	rightLine.Hide();
	leftLine.Hide();
}
