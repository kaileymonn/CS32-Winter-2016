#include "Map.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

Map::Map()
{
	m_size = 0; //Empty Map
	head = NULL; 
	tail = NULL;
}

Map::~Map()
{
	if (!empty()) //empty map check
	{ 
		if (head == tail) //only 1 node
			delete head;
		else //more than 1 node
		{
			mapNode* temp = head; //create temp node pointer
			while (temp->next != NULL) //traverse nodes
			{
				temp = temp->next; //set node to point to next node
				delete temp->prev; //delete previous node
			}
			delete tail; //delete final remaining node
		}
	}
}

Map::Map(const Map& other)
{
	m_size = 0;
	head = NULL;
	tail = NULL;

	for (int i = 0; i < other.size(); i++) //Traverse nodes in other using get function
	{
		KeyType k;
		ValueType v;
		other.get(i, k, v);
		insert(k, v); //Copy over each node
	}
}

Map& Map::operator=(const Map& rhs)
{
	if (this != &rhs) //lhs and rhs are different
	{
		Map temp(rhs); //use copy constructor to create a temp map = rhs
		swap(temp); //lhs now becomes rhs value
	}
	return *this; //allow for multiple iterations of = operator
}

bool Map::empty() const
{
	if (head == NULL)
		return true;
	return false;
}

int Map::size() const //number of nodes
{
	return m_size;
}

bool Map::insert(const KeyType& key, const ValueType& value) 
{
	if (contains(key)) //map already has a node with key value 
		return false;
	mapNode* newNode = new mapNode; //Make new node
	newNode->pair.key = key; //set key value
	newNode->pair.value = value; //set value value
	newNode->next = NULL; //set default pointer
	newNode->prev = NULL; //set default pointer

	if (empty()) //empty map
	{
		head = newNode;
		tail = newNode;
	}
	else //add to end of list
	{
		newNode->prev = tail; //set new node's prev pointer
		tail->next = newNode; //set precursor node's next pointer
		tail = newNode; //set tail to point to new last node
	}
	m_size++; //Increment number of nodes
	return true;
}

bool Map::update(const KeyType& key, const ValueType& value)
{
	if (!contains(key)) //Map doesnt contain node with key value
		return false;

	mapNode* temp = head; //create temp pointer that starts at head node
	
	while (temp->pair.key != key) //traverse nodes to find match
		temp = temp->next;
	temp->pair.value = value; //found matching node
	return true;
}

bool Map::insertOrUpdate(const KeyType& key, const ValueType& value)
{
	if (contains(key)) //Map already has node with key value
		update(key, value);
	else //make a new node
		insert(key, value);

	return true;
}

bool Map::erase(const KeyType& key)
{
	if (!contains(key)) //map doesnt have node with key value or is empty
		return false;

	mapNode* temp = head;

	if (size() == 1) //only 1 node, and it matches
	{
		delete head;
		head = NULL;
		tail = NULL;
		m_size--;
		return true;
	}

	while (temp->pair.key != key) //traverse nodes to find match
		temp = temp->next; //temp now points to node to be deleted

	if (temp == head) //match at 1st node
	{
		head = temp->next;
		delete temp;
		head->prev = NULL;
	}

	else if (temp == tail)	//match at last node
	{
		tail = temp->prev;
		delete temp;
		tail->next = NULL;
	}

	else //match somewhere in the middle
	{
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		delete temp;
	}
	m_size--;
	return true;
}

bool Map::contains(const KeyType& key) const
{
	mapNode* temp = head;
	if (head == NULL) //empty map
		return false;

	while (temp != NULL) //traverse nodes
	{
		if (temp->pair.key == key) //found match
			return true;
		temp = temp->next;
	}
	return false; //no matches found

}

bool Map::get(const KeyType& key, ValueType& value) const
{
	if (!contains(key)) //no matches
		return false;

	mapNode* temp = head; //make temp node to traverse map
	
	while (temp->pair.key != key) //traverse nodes until match
		temp = temp->next;

	value = temp->pair.value;
	return true;
}

bool Map::get(int i, KeyType& key, ValueType& value) const
{
	if (i < 0 || i >= size()) //invalid i input
		return false;

	mapNode* temp = head; //create temp pointer to traverse map

	for (int k = 0; k < i; k++) 
		temp = temp->next; //traverse to ith node in map

	key = temp->pair.key;
	value = temp->pair.value;

	return true;
}

void Map::swap(Map& other)
{
	int thisSize = size(); //save this map's size

	mapNode* tempHead = other.head; //temp pointer to other map's head
	mapNode* tempTail = other.tail; //temp pointer to other map's tail

	other.head = head; //other map's head is now this map's
	other.tail = tail; //other map's tail is now this map's

	head = tempHead; //this map's head is now other's original head
	tail = tempTail; //this map's tail is now other's original tail

	m_size = other.m_size; //set this map's size to other's after swap
	other.m_size = thisSize; //set other map's size to this's after swap
}

bool combine(const Map& m1, const Map& m2, Map& result)
{
	Map m1temp = m1;
	Map m2temp = m2;
	bool weird = true; //to check for mismatch in value
	
	while(!empty(result)) //result map is not empty so clear it
	{
		KeyType k;
		ValueType v;
		result.get(0, k, v);
		result.erase(k);		
	}

	for (int i = 0; i < m1temp.size(); i++) //traverse m1 to check
	{ 
		KeyType k;
		ValueType v;
		m1temp.get(i, k, v);
		if (!m2temp.contains(k)) //non-matching keys, add to result
			result.insert(k, v);
		else //matching keys, check value
		{
			ValueType v2;
			m2temp.get(k, v2); //get m2's value
			if (v == v2) //they match, add to result
				result.insert(k, v2);
			else //same key different value:(
				weird = false;					
		}
	}

	for (int i = 0; i < m2temp.size(); i++) //now traverse m2 (keys that only appear in m2)
	{
		KeyType k;
		ValueType v;
		m2temp.get(i, k, v);
		if (!m1temp.contains(k))
			result.insert(k, v);
		//don't have to check for matching keys since it has already been done earlier
	}
	return weird;
}

void subtract(const Map& m1, const Map& m2, Map& result) //use algo from combine
{
	Map m1temp = m1; //fixes aliasing problem
	Map m2temp = m2;

	while (!empty(result)) //result map is not empty so clear it
	{
		KeyType k;
		ValueType v;
		result.get(0, k, v);
		result.erase(k);
	}
	
	for (int i = 0; i < m1temp.size(); i++) //traverse m1 to check
	{
		KeyType k;
		ValueType v;
		m1temp.get(i, k, v);
		if (!m2temp.contains(k)) //non-matching keys, add to result
			result.insert(k, v);
	}
}