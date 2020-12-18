#pragma once
#include "Console.h"

// Class Prototype

class ATHElement {

public:
	// Variables
	Vector2_int pos;
	Vector2_int size;

	// Constructors
	ATHElement(Vector2_int _pos = Vector2_int(0, 0),  Vector2_int _size = Vector2_int(1, 1));

	// Functions
	//virtual void Show();
	//vitrual void Hide();
};

class VLine: public ATHElement {
public:
	int size;

	// Constructors
	VLine(Vector2_int _pos = Vector2_int(0, 0), int _size = 1);

	// Functions
	void Show();
	void Hide();
};

class HLine : public VLine {
public:
	// Constructors
	HLine(Vector2_int _pos = Vector2_int(0, 0), int _size = 1);

	// Functions
	void Show();
	void Hide();
};

class Rect : public ATHElement {
public:
	HLine topLine, botLine;
	VLine leftLine, rightLine;

	// Constructors
	Rect(Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1));

	// Functions
	void Show();
	void Hide();
};