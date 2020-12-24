#pragma once


class Packet
{
protected:
	//static
	static size_t MAXSIZE;
	// gestion de la taille et de la capacit�
	size_t _capacity;
	size_t _size;
	size_t _cursor;

	// donn�es
	char* _data;

public:
	//static function
	static void setMaxSize(size_t maxSize) { MAXSIZE = maxSize; }
	//constructeur et destructeur
	Packet(size_t beginCapacity = Packet::MAXSIZE); //constructeur avec capacit� de d�part
	~Packet();

	//modification de la taille des donn�es
	void setCapacity(size_t newCapacity); //change la capacit� du packet

	//lecture des attributs
	const size_t size() { return _size; }
	const size_t capacity() { return _capacity; }
	const size_t cursor() { return _cursor; }
	char* data() { return _data; }

	//navigation dans les donn�es
	const bool end() { return _cursor == _size; } //retourne vrai si le curseur est � la fin des donn�es
	void move(size_t);//d�place le curseur

	//modification
	void add(char* newData, size_t dataSize); //ajoute les donn�es � partir de la position du curseur et d�place le curseur � la fin des donn�es


	//operateur pour �criture


	template <typename T>
	Packet& operator << (T data);

	//Packet& operator << (float data);

	//operateur pour lecture
	template <typename T>
	Packet& operator >> (T& data);

	//Packet& operator >> (float& data);


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

#pragma endregion

