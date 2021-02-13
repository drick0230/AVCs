#pragma once
#include <string> //std::string
#include <vector> //std::vector
#include <codecvt> //std::wstring_convert


std::vector <std::string> split(std::string chaine, const char separateur, bool separateurMultiple = false);

template <class T>
T myParse(std::string _string) {
	T _return = 0;

	if (_string[0] >= '0' && _string[0] <= '9') {
		_return = _string[0] - '0';

		unsigned i = 1;
		while (_string[i] >= '0' && _string[i] <= '9') {
			_return *= 10;
			_return += _string[i] - '0';
			i++;
		}
	}
	else {
		std::cout << "Error : < T myParse(std::string _string) > invalid string _string";
		return -1;
	}

	return _return;
}

enum ServerCommand : unsigned char
{
	createRoom = 0,
	joinRoom = 1,
	ExitRoom = 2,
	print = 3,
	serverInfo = 4,
};

enum ClientCommand : unsigned char
{
	addUser = 0,
	username = 1,
	removeUser = 2
};

static std::wstring ToWstring(std::string str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter;
	return strconverter.from_bytes(str);
}

static std::string ToString(std::wstring str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> strconverter;
	return strconverter.to_bytes(str);
}

template <class T>
void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

template <class T>
void SafeDelete(T** ppT) {
	if (*ppT != NULL) {
		delete* ppt;
		*ppt = NULL;
	}
}