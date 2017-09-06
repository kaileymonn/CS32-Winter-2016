#include "WeightMap.h"
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

WeightMap::WeightMap()       // Create an empty weight map.
{
	Map m_weightMap;
}

bool WeightMap::enroll(std::string name, double startWeight)
// If a person with the given name is not currently in the map, 
// there is room in the map, and the startWeight is not negative,
// add an entry for that person and weight and return true.
// Otherwise make no change to the map and return false.
{
	if (m_weightMap.contains(name) || startWeight < 0 || size() >= DEFAULT_MAX_ITEMS)
		return false;
	m_weightMap.insert(name, startWeight);
	return true;	
}

double WeightMap::weight(std::string name) const
// If a person with the given name is in the map, return that
// person's weight; otherwise, return -1.
{
	double weight;
	bool actualPerson = m_weightMap.get(name, weight);

	if (!actualPerson)
		return -1;
	return weight;
}

bool WeightMap::adjustWeight(std::string name, double amt)
// If no person with the given name is in the map or if
// weight() + amt is negative, make no change to the map and return
// false.  Otherwise, change the weight of the indicated person by
// the given amount and return true.  For example, if amt is -8.2,
// the person loses 8.2 pounds; if it's 3.7, the person gains 3.7
// pounds.
{
	if (!m_weightMap.contains(name) || (weight(name) + amt) < 0)
		return false;
	double oldWeight;
	m_weightMap.get(name, oldWeight);
	double newWeight = oldWeight + amt;
	m_weightMap.update(name, newWeight);
	return true;
}

int WeightMap::size() const  // Return the number of people in the WeightMap.
{
	return m_weightMap.size();
}

void WeightMap::print() const
// Write to cout one line for every person in the map.  Each line
// has the person's name, followed by one space, followed by that
// person's weight.
{
	for (int i = 0; i < size(); i++)
	{
		string name;
		double weight;

		m_weightMap.get(i, name, weight);
		cout << name << " " << weight << endl;
	}
}