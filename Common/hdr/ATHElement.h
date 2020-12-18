#pragma once
#include "Console.h"

namespace ATH {
	// Class Prototype

	class ATHElement {
	protected:
		// Variables
		Vector2_int pos;
		Vector2_int size;
		Color color;

	public:
		// Variables
		std::vector<ATHElement*> childs;
		ATHElement* parent;

		// Constructors
		ATHElement(Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _color = Color(255, 255, 255));
		ATHElement(ATHElement* _parent, Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _color = Color(255, 255, 255));

		// Functions
		void AddChild(ATHElement* _element);
		virtual void Show(bool _childsShow = true);
		virtual void Hide();
	};

	class VLine : public ATHElement {
	public:
		// Constructors
		VLine(Vector2_int _pos = Vector2_int(0, 0), int _size = 1, Color _color = Color(255, 255, 255));
		VLine(ATHElement* _parent, Vector2_int _pos = Vector2_int(0, 0), int _size = 1, Color _color = Color(255, 255, 255));

		// Functions
		virtual void Show(bool _childsShow = true);
		virtual void Hide();
	};

	class HLine : public VLine {
	public:
		// Constructors
		HLine(Vector2_int _pos = Vector2_int(0, 0), int _size = 1, Color _color = Color(255, 255, 255));
		HLine(ATHElement* _parent, Vector2_int _pos = Vector2_int(0, 0), int _size = 1, Color _color = Color(255, 255, 255));

		// Functions
		void Show(bool _childsShow = true);
		void Hide();
	};

	class OutlinedRect : public ATHElement {
	public:
		HLine topLine, botLine;
		VLine leftLine, rightLine;

		// Constructors
		OutlinedRect(Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _color = Color(255, 255, 255));
		OutlinedRect(ATHElement* _parent, Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _color = Color(255, 255, 255));

		// Functions
		void Show(bool _childsShow = true);
		void Hide();
	};

	class FilledRect : public ATHElement {
	public:
		// Constructors
		FilledRect(Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _color = Color(255, 255, 255));
		FilledRect(ATHElement* _parent, Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _color = Color(255, 255, 255));

		// Functions
		virtual void Show(bool _childsShow = true);
		virtual void Hide();
	};

	class Rect : public FilledRect {
	public:
		OutlinedRect outline;

		// Constructors
		Rect(Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _filledColor = Color(255, 255, 255), Color _outlineColor = Color(255, 255, 255));
		Rect(ATHElement* _parent, Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _filledColor = Color(255, 255, 255), Color _outlineColor = Color(255, 255, 255));

		// Functions
		void Show(bool _childsShow = true);
		void Hide();
	};

	class SimpleText : public FilledRect {
	private:
		unsigned int nbLine, nbCharInLine;
		std::string content;
		Color fontColor;
	public:
		// Constructors
		SimpleText(std::string _content = "", Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _fontColor = Color(255, 255, 255), Color _backgroundColor = Color(255, 255, 255));
		SimpleText(ATHElement* _parent, std::string _content = "", Vector2_int _pos = Vector2_int(0, 0), Vector2_int _size = Vector2_int(1, 1), Color _fontColor = Color(255, 255, 255), Color _backgroundColor = Color(255, 255, 255));
	

		// Functions
		bool Write(char _c); // return false if the limit is reach
		bool Write(std::string _s); // return false if the limit is reach

		void Show(bool _childsShow = true);
	};
}