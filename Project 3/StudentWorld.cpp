#include <iostream>
#include <vector>
#include "actor.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <ctime>

using namespace std;

StudentWorld::StudentWorld(string assetDir) : GameWorld(assetDir), m_counter(0) {} 

StudentWorld::~StudentWorld()
{
	for (int i = 0; i < m_actor.size(); i++) //cycle through vector to delete all actors
		delete m_actor.at(i);
	m_actor.clear(); //clear vector
}

int StudentWorld::init()
{
	srand(static_cast<unsigned int>(time(0))); //seed random operator with time

	Frackman * player = new Frackman(this); //create new frackman and put into vecctor
	m_actor.push_back(player);

	for (int i = 0; i < 30; i++) //Fill up left side with dirt
		for (int k = 0; k < 60; k++)
		{
			Dirt* poop = new Dirt(this, i, k);
			m_actor.push_back(poop);
		}

	for (int i = 34; i < 60; i++) //Fill up RIGHT side with dirt
		for (int k = 0; k < 60; k++)
		{
			Dirt* poop = new Dirt(this, i, k);
			m_actor.push_back(poop);
		}
	return GWSTATUS_CONTINUE_GAME;

}

int StudentWorld::move()
{
	m_counter++;

	//Get all actors to do something if alive
	for (int i = 0; i < m_actor.size(); i++)
		if (m_actor.at(i)->isStillAlive())
			m_actor.at(i)->doSomething();

	//Remove dead dirt
	for (int i = 0; i < m_actor.size(); i++)
		if (!m_actor.at(i)->isStillAlive())
		{
			delete m_actor.at(i);

			vector<GameObject*>::iterator it = m_actor.begin(); //create an iterator for the pointer index to be erased
			for (int j = 0; j < i; j++)
				it++;
			m_actor.erase(it);
			i--;

		}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	for (int i = 0; i < m_actor.size(); i++)
		delete m_actor.at(i); //delete all actors
	m_actor.clear(); //clear vector
}

bool StudentWorld::dirtThere(int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++)
	{
		int id = m_actor.at(i)->getID();
		if (id == IID_DIRT) //for each dirt in the world
		{
			int tempx, tempy; //get its coordinates
			tempx = m_actor.at(i)->getX();
			tempy = m_actor.at(i)->getY();

			if (x == tempx && y == tempy) //if there is dirt at the parameter x,y, return true
				return true;
		}
	}
	return false;
}

bool StudentWorld::FrackmanThere(int x, int y)
{
	for (int i = 0; i < m_actor.size(); i++)
	{
		int id = m_actor.at(i)->getID();
		if (id == IID_PLAYER) //if player actor
		{
			int tempx, tempy; //get its coordinates
			tempx = m_actor.at(i)->getX();
			tempy = m_actor.at(i)->getY();

			if (x == tempx && y == tempy) //if there is dirt at the parameter x,y, return true
				return true;
		}
	}
	return false;
}

void StudentWorld::storeObject(GameObject* temp)
{
	m_actor.push_back(temp); //push GameObject pointer into the vector
}

void StudentWorld::removeDirt(int x, int y)
{
	int xx, yy;
	for (int i = 0; i<m_actor.size(); i++)
	{
		xx = m_actor.at(i)->getX; //if dirt is at x,y
		yy = m_actor.at(i)->getY;
		if (xx == x && yy == y && (m_actor.at(i)->getID() == IID_DIRT))
			m_actor.at(i)->setDead(); //set it to dead
	}
}

void StudentWorld::killPlayer(int x, int y)
{
	int xx, yy;
	for (int i = 0; i<m_actor.size(); i++)
	{
		xx = m_actor.at(i)->getX;
		yy = m_actor.at(i)->getY;
		if (xx == x && yy == y && m_actor.at(i)->getID() == IID_PLAYER) //if player is at x,y
			m_actor.at(i)->setDead(); //set the player to dead
	}
}

int StudentWorld::randInt(int lowest, int highest)
{
	if (highest < lowest)
		swap(highest, lowest); //organize the numbers in times of size
	return lowest + (rand() % (highest - lowest + 1)); //return the corresponding random integer
}