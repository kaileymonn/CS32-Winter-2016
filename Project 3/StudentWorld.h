#ifndef StudentWorld_H
#define StudentWorld_H

#include <algorithm>
#include <vector>
#include "GameWorld.h"

class GameObject;

class StudentWorld : public GameWorld
{
public:
	StudentWorld::StudentWorld(std::string assetDir);
	~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();

	//Checks if objects are at location x,y
	bool dirtThere(int x, int y); 
	bool FrackmanThere(int x, int y);
	
	//Mods
	void storeObject(GameObject* temp); //add GameObject pointer to private vector
	void removeDirt(int x, int y); //remove dirt at x,y
	void killPlayer(int x, int y); //player died:(

	//Adds

private:
	std::vector<GameObject*> m_actor;
	int m_counter;
	int randInt(int lowest, int highest);
};

#endif // #ifndef StudentWorld_H