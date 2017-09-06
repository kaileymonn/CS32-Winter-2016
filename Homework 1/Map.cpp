#include "Map.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

Map::Map() // Create an empty map (i.e., one with no key/value pairs)
{
	lastIndex = -1;
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
	if (size() >= DEFAULT_MAX_ITEMS)
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
	dataPair temp[DEFAULT_MAX_ITEMS];
	int mySize = size();
	int otherSize = other.size();

	for (int i = 0; i < otherSize; i++) //Copy other to temp
	{
		KeyType k;
		ValueType v;
		other.get(i, k, v);

		temp[i].key = k;
		temp[i].value = v;
	}

	while (!other.empty()) //Empty other map
	{
		KeyType k;
		ValueType v;

		other.get(0, k, v);
		other.erase(k); 
	}

	for (int i = 0; i < mySize; i++) //Copy this map to other
	{
		KeyType k;
		ValueType v;

		get(i, k, v);
		other.insert(k, v);
	}

	while (!empty()) //Empty this map
	{
		KeyType k;
		ValueType v;

		get(0, k, v);
		erase(k);
	}

	for (int i = 0; i < otherSize; i++) //Copy temp to this map
	{
		insert(temp[i].key, temp[i].value);
	}
}