#include "socket.h"

#include "Packet.h"

size_t Packet::MAXSIZE = 512;

#pragma region méthode

Packet::Packet(size_t beginCapacity) :_capacity(beginCapacity), _size(0), _cursor(0)
{
	_data = new char[_capacity];
	for (size_t i = 0; i < _capacity; i++)_data[i] = 0;
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
	delete[] _data;
	_data = new_data;
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

#pragma endregion
