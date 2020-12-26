#pragma once
#include <string>

class Packet
{
protected:
	//static
	static size_t MAXSIZE;
	// gestion de la taille et de la capacité
	size_t _capacity;
	size_t _size;
	size_t _cursor;

	// données
	char* _data;

public:
	//static function
	static void setMaxSize(size_t maxSize) { MAXSIZE = maxSize; }
	//constructeur et destructeur
	Packet(size_t beginCapacity = Packet::MAXSIZE); //constructeur avec capacité de départ
	Packet(const Packet &base);
	~Packet();

	//modification de la taille des données
	void setCapacity(size_t newCapacity); //change la capacité du packet

	//lecture des attributs
	const size_t size() { return _size; }
	const size_t capacity() { return _capacity; }
	const size_t cursor() { return _cursor; }
	char* data() { return _data; }

	//navigation dans les données
	const bool end() { return _cursor == _size; } //retourne vrai si le curseur est à la fin des données
	void move(size_t);//déplace le curseur

	//modification
	void add(char* newData, size_t dataSize); //ajoute les données à partir de la position du curseur et déplace le curseur à la fin des données


	//operateur pour écriture

	Packet& operator = (const Packet& _b);

	template <typename T>
	Packet& operator << (T data);
	template <>
	Packet& operator << (std::string data);


	//Packet& operator << (float data);

	//operateur pour lecture

	template <typename T>
	Packet& operator >> (T& data);
	template <>
	Packet& operator >> (std::string& data);
};



#pragma region << operator
template <typename T>
Packet& Packet::operator << (T data)
{
	const size_t bytesNbr = sizeof(T);

	char* cdata = (char*)&data;
	add(cdata, bytesNbr);
	return *this;
}

template <>
Packet& Packet::operator << (std::string data)
{
	const size_t bytesNbr = data.size() + 1;

	char* cdata = (char*)data.c_str();

	add(cdata, bytesNbr);
	return *this;
}

#pragma endregion

#pragma region >> operator

template <typename T>
Packet& Packet::operator >> (T& data)
{
	const size_t bytesNbr = sizeof(T);
	if ((_cursor + bytesNbr) > _size)throw "depassement lors de la lecture";

	data = *(T*)(_data + _cursor);
	_cursor += bytesNbr;
	return *this;
}

template <>
Packet& Packet::operator >> (std::string& data)
{
	size_t bytesNbr = 0;
	const size_t virtSize = _size - 1;
	while (*(_data + _cursor + bytesNbr ) != 0)
	{
		bytesNbr++;
		if ((_cursor + bytesNbr) > virtSize)throw "depassement lors de la lecture";
	}

	data = std::string(_data + _cursor);
	_cursor += bytesNbr + 1;
	return *this;
}

#pragma endregion

