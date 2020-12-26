#include "Packet.h"

size_t Packet::MAXSIZE = 512;

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
	if (newCapacity > MAXSIZE)throw "depassement de MAXSIZE";
	if ((new_data = new char[newCapacity]) == nullptr)throw "echec de l'assignation";
	size_t limit = _size;
	if (newCapacity < limit) limit = newCapacity;
	for (size_t i = 0; i < limit; i++)
	{
		new_data[i] = _data[i];
	}
	delete[_capacity] _data;
	_data = new_data;
	new_data = nullptr;
	_capacity = newCapacity;
}

void Packet::move(size_t position)
{
	if (position > _size) throw "deplacement à l'exterieur des donnees";
	_cursor = position;
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

Packet& Packet::operator = (const Packet& _b) {
	delete[] _data;
	_data = new char[_capacity];

	_capacity = _b._capacity;
	_size = _b._size;
	_cursor = _b._cursor;

	for (size_t i = 0; i < _capacity; i++)
		_data[i] = _b._data[i];
	return *this;
}



#pragma endregion
