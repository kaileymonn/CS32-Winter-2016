#include <iostream>
#include "actor.h"
#include "StudentWorld.h"
#include "GameController.h"


GameObject::GameObject(const int id, StudentWorld*world, int startx, int starty, Direction dir, double size, unsigned int health)
	: GraphObject(id, startx, starty, dir, size, health), myWorld(world), m_alive(true)
{
	setVisible(true);
}

GameObject::~GameObject()
{
	setVisible(false);
}

bool GameObject::isStillAlive() //check if object still alive
{
	return m_alive; 
}

void GameObject::setDead() //dead
{
	m_alive = false;	
}

StudentWorld* GameObject::getWorld() const
{
	return myWorld;
}

int GameObject::randInt(int lowest, int highest) //returns a random int
{
	if (highest < lowest) swap(highest, lowest);
	return lowest + (rand() % (highest - lowest + 1));
}

//////////////////////////////////////////////////////////////

Dirt::Dirt(StudentWorld* world, int x, int y) : GameObject(IID_DIRT, world, x, y, right, 0.25, 1)
{
	m_gotDug = false;
}

Dirt::~Dirt() {}

void Dirt::doSomething() //if it got dug, dig it up
{
	if (m_gotDug == true) setDead(); 
}

void Dirt::gotDug() //sets m_gotDug to true
{
	m_gotDug = true;
}

//////////////////////////////////////////////////////////////

Frackman::Frackman(StudentWorld* world) : GameObject(IID_PLAYER, world, 30, 60, right, 1, 1)
{
	m_squirts = 5;
}

Frackman::~Frackman() {}

void Frackman::doSomething()
{
	int curx, cury, dir;
	curx = getX();//get current locations
	cury = getY();

	if (getWorld()->getKey(dir) == true) //direction input
	{
		if (dir == KEY_PRESS_LEFT && curx != 0) //new position within bounds LEFT
		{
			if (getWorld()->dirtThere(curx - 1, cury)) //there is dirt in new position
			{
				getWorld()->removeDirt(curx-1, cury);
				moveTo(curx - 1, cury);
			}
			else
				moveTo(curx - 1, cury);
		}
		else if (dir == KEY_PRESS_RIGHT && curx != 60) //RIGHT
		{
			if (getWorld()->dirtThere(curx + 1, cury))
			{
				getWorld()->removeDirt(curx + 1, cury);
				moveTo(curx + 1, cury);
			}
			else
				moveTo(curx + 1, cury);
		}
		else if (dir == KEY_PRESS_UP && cury != 60) //UP
		{
			if (getWorld()->dirtThere(curx, cury+1))
			{
				getWorld()->removeDirt(curx, cury+1);
				moveTo(curx, cury+1);
			}
			else
				moveTo(curx, cury+1);
		}
		else if (dir == KEY_PRESS_DOWN && cury != 0) //DOWN
		{
			if (getWorld()->dirtThere(curx, cury - 1))
			{
				getWorld()->removeDirt(curx, cury - 1);
				moveTo(curx, cury - 1);
			}
			else
				moveTo(curx, cury - 1);
		}
	}
}


