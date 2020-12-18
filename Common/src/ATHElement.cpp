#include "ATHElement.h"
using namespace ATH;
/////////////////////////// Constructors /////////////////////////////////////
ATHElement::ATHElement(Vector2_int _pos, Vector2_int _size, Color _color) : ATHElement(NULL, _pos, _size, _color) {}
ATHElement::ATHElement(ATHElement* _parent, Vector2_int _pos, Vector2_int _size, Color _color) : parent(_parent), pos(_pos), size(_size), color(_color) {
	if (parent != NULL)
		parent->AddChild(this);
}

VLine::VLine(Vector2_int _pos, int _size, Color _color) : VLine(NULL, _pos, _size, _color) {}
VLine::VLine(ATHElement* _parent, Vector2_int _pos, int _size, Color _color) : ATHElement(_parent, _pos, _size, _color) {}


HLine::HLine(Vector2_int _pos, int _size, Color _color) : HLine(NULL, _pos, _size, _color) {}
HLine::HLine(ATHElement* _parent, Vector2_int _pos, int _size, Color _color) : VLine(_parent, _pos, _size, _color) {}

OutlinedRect::OutlinedRect(Vector2_int _pos, Vector2_int _size, Color _color) : OutlinedRect(NULL, _pos, _size, _color) {}
OutlinedRect::OutlinedRect(ATHElement* _parent, Vector2_int _pos, Vector2_int _size, Color _color) :
	ATHElement(_parent, _pos, _size, _color),
	topLine(_pos, _size.x, _color),
	leftLine(Vector2_int(_pos.x, pos.y + 1), _size.y - 1, _color),
	rightLine(Vector2_int(_pos.x + size.x - 1, pos.y + 1), _size.y - 1, _color),
	botLine(Vector2_int(_pos.x + 1, pos.y + size.y - 1), _size.x - 2, _color) {}

FilledRect::FilledRect(Vector2_int _pos, Vector2_int _size, Color _color) : FilledRect(NULL, _pos, _size, _color) {}
FilledRect::FilledRect(ATHElement* _parent, Vector2_int _pos, Vector2_int _size, Color _color) : ATHElement(_parent, _pos, _size, _color) {}

Rect::Rect(Vector2_int _pos, Vector2_int _size, Color _filledColor, Color _outlineColor) : Rect(NULL, _pos, _size, _filledColor, _outlineColor) {}
Rect::Rect(ATHElement* _parent, Vector2_int _pos, Vector2_int _size, Color _filledColor, Color _outlineColor) :
	FilledRect(_parent, Vector2_int(_pos.x + 1, _pos.y + 1), Vector2_int(_size.x - 2, _size.y - 2), _filledColor),
	outline(_pos, _size, _outlineColor) {}

SimpleText::SimpleText(std::string _content, Vector2_int _pos, Vector2_int _size, Color _fontColor, Color _backgroundColor) : SimpleText(NULL, _content, _pos, _size, _fontColor, _backgroundColor) {}
SimpleText::SimpleText(ATHElement* _parent, std::string _content, Vector2_int _pos, Vector2_int _size, Color _fontColor, Color _backgroundColor) : FilledRect(_parent, _pos, _size, _backgroundColor), content(""), fontColor(_fontColor), nbLine(1), nbCharInLine(0) {
	Write(_content);
}

/////////////////////////// Functions /////////////////////////////////////
void ATHElement::AddChild(ATHElement* _element) {
	childs.push_back(_element);
}

void ATHElement::Show(bool _childsShow) {
	if (_childsShow)
		for (unsigned int _i = 0; _i < childs.size(); _i++)
			childs[_i]->Show();
}

void ATHElement::Hide() {
	for (unsigned int _i = 0; _i < childs.size(); _i++)
		childs[_i]->Hide();
}

void VLine::Show(bool _childsShow) {
	Console::SetFontColor(color);
	for (int _i = 0; _i < size.x; _i++) {
		Console::GoTo(pos.x, pos.y + _i);
		printf_s("%c", (char)219);
	}
	ATHElement::Show(_childsShow);
}

void VLine::Hide() {
	Console::DefaultBackgroundColor();
	for (int _i = 0; _i < size.x; _i++) {
		Console::GoTo(pos.x, pos.y + _i);
		printf_s("%c", ' ');
	}
	ATHElement::Hide();
}

void HLine::Show(bool _childsShow) {
	Console::SetFontColor(color);
	Console::GoTo(pos.x, pos.y);
	for (unsigned char _i = 0; _i < size.x; _i++)
		printf_s("%c", (char)219);
	ATHElement::Show(_childsShow);
}

void HLine::Hide() {
	Console::DefaultBackgroundColor();
	Console::GoTo(pos.x, pos.y);
	for (unsigned char _i = 0; _i < size.x; _i++)
		printf_s("%c", ' ');
	ATHElement::Hide();
}

void OutlinedRect::Show(bool _childsShow) {
	topLine.Show(false);
	botLine.Show(false);
	rightLine.Show(false);
	leftLine.Show(false);
	ATHElement::Show(_childsShow);
}

void OutlinedRect::Hide() {
	topLine.Hide();
	botLine.Hide();
	rightLine.Hide();
	leftLine.Hide();
	ATHElement::Hide();
}

void FilledRect::Show(bool _childsShow) {
	Console::SetFontColor(color);
	for (unsigned int _y = 0; _y < size.y; _y++) {
		Console::GoTo(pos.x, pos.y + _y);
		for (unsigned int _x = 0; _x < size.x; _x++)
			printf_s("%c", (char)219);
	}
	ATHElement::Show(_childsShow);
}

void FilledRect::Hide() {
	Console::DefaultBackgroundColor();
	for (unsigned int _y = 0; _y < size.y; _y++) {
		Console::GoTo(pos.x, pos.y + _y);
		Console::EraseChar(size.x);
	}
	ATHElement::Hide();
}

void Rect::Show(bool _childsShow) {
	outline.Show(false);
	FilledRect::Show(_childsShow);
}

void Rect::Hide() {
	Console::DefaultBackgroundColor();
	for (unsigned int _y = 0; _y < size.y + 2; _y++) {
		Console::GoTo(pos.x - 1, pos.y + _y - 1);
		Console::EraseChar(size.x + 2);
	}
	ATHElement::Hide();
}

bool SimpleText::Write(char _c) {
	if (_c == '\n')
		if (nbLine < size.y) {
			content.push_back(_c);
			nbCharInLine = 0;
			nbLine++;
		}
		else
			return false;
	else if (nbCharInLine < size.x)
	{
		content.push_back(_c);
		nbCharInLine++;
	}
	else
		return false;

	return true;
}

bool SimpleText::Write(std::string _s) {
	unsigned int _i = 0;
	while (_s[_i] != '\0') {
		if (!Write(_s[_i]))
			return false;
		_i++;
	}
	return true;
}

void SimpleText::Show(bool _childsShow) {
	FilledRect::Show(false);
	Console::SetFontColor(fontColor);
	Console::SetBackgroundColor(color);
	unsigned int _i = 0;
	for (unsigned int _y = 0; _y < nbLine; _y++) {
		Console::GoTo(pos.x, pos.y + _y);
		while (content[_i] != '\0' && content[_i] != '\n') {
			printf_s("%c", content[_i]);
			_i++;
		}
		_i++;
	}
	ATHElement::Show(_childsShow);
}