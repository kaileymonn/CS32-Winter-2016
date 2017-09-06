#include "newMap.h"	
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

Map::Map()
{
	lastIndex = -1;
	m_maxSize = DEFAULT_MAX_ITEMS;
	pairs = new dataPair[DEFAULT_MAX_ITEMS];
}

Map::Map(int max)
{
	if (max < 0)
	{
		cout << "Map size must be positive." << endl;
		exit(1);
	}

	lastIndex = -1;
	m_maxSize = max;
	pairs = new dataPair[m_maxSize];
}

Map::Map(const Map& m)
{
	m_maxSize = m.m_maxSize;
	lastIndex = -1;
	pairs = new dataPair[m_maxSize];

	for (int i = 0; i < m.size(); i++)
	{
		KeyType k;
		ValueType v;

		m.get(i, k, v);
		insert(k, v);
	}
}

Map::~Map()
{
	delete[] pairs;
}

Map& Map::operator=(const Map& ao)
{
	if (&ao == this)
		return(*this);
	Map temp(ao);

	m_maxSize = temp.m_maxSize;
	lastIndex = -1;
	pairs = new dataPair[temp.m_maxSize];

	for (int i = 0; i < temp.size(); i++)
	{
		KeyType k;
		ValueType v;

		temp.get(i, k, v);
		insert(k, v);
	}
	return (*this);
}

bool Map::empty() const // Return true if the map is empty, otherwise false.
{
	return size() == 0;
}

int Map::size() const // Return the number of key/value pairs in the map.
{
	return lastIndex + 1;
}

bool Map::insert(const KeyType& key, const ValueType& value)
// If key is not equal to any key currently in the map, and if the
// key/value pair can be added to the map, then do so and return true.
// Otherwise, make no change to the map and return false (indicating
// that either the key is already in the map, or the map has a fixed
// capacity and is full.
{
	if (size() >= m_maxSize)
		return false;
	for (int i = 0; i < size(); i++)
		if (pairs[i].key == key)
			return false;
	pairs[++lastIndex].key = key;
	pairs[lastIndex].value = value;

	return true;
}

bool Map::update(const KeyType& key, const ValueType& value)
// If key is equal to a key currently in the map, then make that key no
// longer map to the value it currently maps to, but instead map to
// the value of the second parameter; return true in this case.
// Otherwise, make no change to the map and return false.
{
	for (int i = 0; i < size(); i++)
	{
		if (pairs[i].key == key)
		{
			pairs[i].value = value;
			return true;
		}
	}
	return false;
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
// If key is equal to a key currently in the map, then make that key no
// longer map to the value it currently maps to, but instead map to
// the value of the second parameter; return true in this case.
// If key is not equal to any key currently in the map, and if the
// key/value pair can be added to the map, then do so and return true.
// Otherwise, make no change to the map and return false (indicating
// that the key is not already in the map and the map has a fixed
// capacity and is full.
{
	if (update(key, value))
		return true;
	else
		return insert(key, value);
}

bool Map::erase(const KeyType& key)
// If key is equal to a key currently in the map, remove the key/value
// pair with that key from the map and return true.  Otherwise, make
// no change to the map and return false.
{
	if (size() <= 1)
	{
		if (!empty() && pairs[0].key == key)
		{
			//pairs[0].key=KeyType(NULL);
			//pairs[0].value=ValueType(NULL);
			lastIndex--;
			return true;
		}
		return false;
	}

	for (int i = 0; i < size(); i++)
	{
		if (pairs[i].key == key)
		{
			pairs[i].key = pairs[size() - 1].key;
			pairs[i].value = pairs[size() - 1].value;
			lastIndex--;
			return true;
		}
	}

	return false;
}

bool Map::contains(const KeyType& key) const
// Return true if key is equal to a key currently in the map, otherwise
// false.
{
	if (empty())
		return false;
	for (int i = 0; i < size(); i++)
	{
		if (pairs[i].key == key)
			return true;
	}
	return false;
}

bool Map::get(const KeyType& key, ValueType& value) const
// If key is equal to a key currently in the map, set value to the
// value in the map that that key maps to and return true.  Otherwise,
// make no change to the value parameter of this function and return
// false.
{
	if (contains(key))
		for (int i = 0; i < size(); i++)
			if (pairs[i].key == key)
			{
				value = pairs[i].value;
				return true;
			}
	return false;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
// If 0 <= i < size(), copy into the key and value parameters the
// key and value of one of the key/value pairs in the map and return
// true.  Otherwise, leave the key and value parameters unchanged and
// return false.  (See below for details about this function.)
{
	if (i >= 0 && i < size())
	{
		key = pairs[i].key;
		value = pairs[i].value;
		return true;
	}
	return false;
}

void Map::swap(Map& other)
// Exchange the contents of this map with the other one.
{
	Map temp = other;
	other = *this;
	*this = temp;
}