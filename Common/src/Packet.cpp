/**
 * @file	Packet.cpp
 * @author	Keven Gagnon
 * @date	2021-02-14
 */

#include "Packet.h"

#pragma region méthode

Packet::Packet(size_t beginCapacity) :_capacity(beginCapacity), _size(0), _cursor(0)
{
	_data = new char[_capacity];
	for (size_t i = 0; i < _capacity; i++)_data[i] = 0;
}

Packet::Packet(const Packet& base) :_capacity(base._capacity), _size(base._size), _cursor(base._cursor)
{
	_data = new char[_capacity];
	for (size_t i = 0; i < _capacity; i++)_data[i] = base._data[i];
}


Packet::~Packet()
{
	delete[] _data;
}

void Packet::setCapacity(size_t newCapacity)
{
	char* new_data = nullptr;
	if ((new_data = new char[newCapacity]) == nullptr)throw "echec de l'assignation";
	size_t limit = _size;
	if (newCapacity < limit) limit = newCapacity;
	for (size_t i = 0; i < limit; i++)
	{
		new_data[i] = _data[i];
	}
	delete[] _data;
	_data = new_data;
	new_data = nullptr;
	_capacity = newCapacity;
}

void Packet::move(size_t position)
{
	if (position > _size) throw "deplacement à l'exterieur des donnees";
	_cursor = position;
}

bool Packet::Peek(std::string _strToFound) {
	if (_cursor + _strToFound.size() > _size) return false;
	if (_data[_cursor + _strToFound.size()] != '\0') return false;
	size_t _oldCursor = _cursor;

	std::string _str;
	*this >> _str;
	_cursor = _oldCursor;
	if (_str == _strToFound)
		return true;

	return false;
}

void Packet::add(char* newData, size_t dataSize)
{
	//modification de la taille et de la capacité
	size_t newSize = _cursor + dataSize;
	if (newSize > _capacity) setCapacity(newSize);
	if (newSize > _size) _size = newSize;
	//ajout des données
	for (size_t i = 0; i < dataSize; i++)
	{
		_data[_cursor] = newData[i];
		_cursor++;
	}
}

void Packet::emplace(const char _newData, size_t _nbData) {
	//modification de la taille et de la capacité
	size_t newSize = _cursor + _nbData;
	if (newSize > _capacity) setCapacity(newSize);
	if (newSize > _size) _size = newSize;
	//ajout des données
	for (size_t i = 0; i < _nbData; i++)
	{
		_data[_cursor] = _newData;
		_cursor++;
	}
}

Packet& Packet::operator = (const Packet& _b) {
	delete[] _data;
	_data = new char[_capacity];

	_capacity = _b._capacity;
	_size = _b._size;
	_cursor = _b._cursor;

	for (size_t i = 0; i < _size; i++)
		_data[i] = _b._data[i];
	return *this;
}

Packet& Packet::operator << (std::string data)
{
	const size_t bytesNbr = data.size() + 1;

	char* cdata = (char*)data.c_str();

	add(cdata, bytesNbr);
	return *this;
}

Packet& Packet::operator >> (std::string& data)
{
	size_t bytesNbr = 0;
	const size_t virtSize = _size - 1;
	while (*(_data + _cursor + bytesNbr) != 0)
	{
		bytesNbr++;
		if ((_cursor + bytesNbr) > virtSize)throw "depassement lors de la lecture";
	}

	data = std::string(_data + _cursor);
	_cursor += bytesNbr + 1;
	return *this;
}

void Packet::popBack(size_t nbr)
{
	if (((long)_size - (long)nbr) < 0)throw "nbr insufisant de data dans le packet";
	_size -= nbr;
	if (_cursor > _size) _cursor = _size;
}

void Packet::read(char* data, size_t readSize)
{
	if ((_cursor + readSize) > _size)throw "depassement lors de la lecture";
	for (int i = 0; i < readSize; i++)
	{
		data[i] = _data[i + _cursor];
	}
	_cursor += readSize;

}
#pragma endregion
