#include "Actor.h"
#include "StudentWorld.h"

using namespace std;

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

/********** ACTOR BASE **********/

/**
Constructor for abstract class Actor
@param int x, int y, bool isVisible, int imageID, Direction dir, double size, unsigned int depth
int x, int y denoting x and y position of the actor, bool isVisible denoting default visibility
imageID one of IID_* consts, a default direction, a double size, an unsigned int depth, and its health
*/
Actor::Actor(int x, int y, bool isVisible, int imageID, Direction dir, double size, unsigned int depth, unsigned int health, StudentWorld* world)
	: GraphObject(imageID, x, y, dir, size, depth)
	, m_isDead(false)
	, m_health(health)
	, m_world(world)
{
	setVisible(isVisible);
};

/**
Actor Destructor
*/
Actor::~Actor() {
	//cerr << "Destroying Actor at x: " << getX() << " | y: " << getY() << endl;
}

/**
Set the actor's state to Dead(if it's not already dead), hides the actor
@return true if successfully sets the actor to dead, false otherwise
*/
bool Actor::setDead() {
	if (m_isDead) return false;

	m_isDead = true;
	setVisible(false);
	return true;
}

/**
Decreases the actor's healthy by n
@return true
*/
bool Actor::decrHealthBy(const int n) {
	m_health -= n;
	return true;
}

/********** FRACKMAN **********/

Frackman::Frackman(StudentWorld* world)
	: Actor(FRACKMAN_SC, FRACKMAN_SR, true, IID_PLAYER, right, 1.0, 0, FRACKMAN_TH, world)
	, m_water_count(5)
	, m_sonar_count(1)
	, m_gold_count(0)
{};


/**
Decreases the frackman's healthy by n
@return true if the decrement does not empty the health
*/
bool Frackman::decrHealthBy(const int n) {
	if (getHealth() - n > 0) {
		cerr << "Frackman got hit! hp: " << getHealth() - n << endl;
		Actor::decrHealthBy(n);
		return true;
	}
	else {
		cerr << "Frackman's hp dropped below 0!" << endl;
		Actor::decrHealthBy(n);
		setDead();
		return false;
	}
}

/**
Handles the move of the player by
changing the player's position based on last key press
@param const int input - the key last entered
*/
void Frackman::handleInput() {
	int ch;
	if (getWorld()->getKey(ch) == true) {
		int p_x = getX();
		int p_y = getY();
		bool isBlocked = false;

		switch (ch) {
		case 'Z':
		case 'z':
			if (m_sonar_count > 0) {
				getWorld()->revealActorsAt(p_x, p_y, 12);
				m_sonar_count--;
			}
			break;
		case KEY_PRESS_TAB:
			dropGold(p_x, p_y);
			break;
		case KEY_PRESS_SPACE:
			createNewSquirt(p_x, p_y, getDirection());
			break;
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_LEFT:
			isBlocked = getWorld()->hasBlockingActorAt(p_x - 1, p_y);
			if (p_x - 1 >= 0 && getDirection() == left && !isBlocked)
				moveTo(--p_x, p_y);
			else if (!isBlocked)
				moveTo(p_x, p_y);
			setDirection(left);
			break;
		case KEY_PRESS_RIGHT:
			isBlocked = getWorld()->hasBlockingActorAt(p_x + 1, p_y);
			if (p_x + 1 <= ACTOR_MAXC && getDirection() == right && !isBlocked)
				moveTo(++p_x, p_y);
			else if (!isBlocked)
				moveTo(p_x, p_y);
			setDirection(right);
			break;
		case KEY_PRESS_DOWN:
			isBlocked = getWorld()->hasBlockingActorAt(p_x, p_y - 1);
			if (p_y - 1 >= 0 && getDirection() == down && !isBlocked)
				moveTo(p_x, --p_y);
			else if (!isBlocked)
				moveTo(p_x, p_y);
			setDirection(down);
			break;
		case KEY_PRESS_UP:
			isBlocked = getWorld()->hasBlockingActorAt(p_x, p_y + 1);
			if (p_y + 1 <= ACTOR_MAXR && getDirection() == up && !isBlocked)
				moveTo(p_x, ++p_y);
			else if (!isBlocked)
				moveTo(p_x, p_y);
			setDirection(up);
			break;
		}

		//cerr << "Player moved to x:" << p_x << " | y: " << p_y << endl;
	}

}

/**
Drops a gold at the specified location and direction
@param int x, int y
*/
void Frackman::dropGold(int x, int y) {
	if (m_gold_count > 0) {
		m_gold_count--;
		Actor* g = new Gold(x, y, true, getWorld());
		getWorld()->addActor(g);
	}
}


/**
Creates a new squirt at the specified location and direction
@param int x, int y, int direction
*/
void Frackman::createNewSquirt(int x, int y, Direction dir) {
	if (m_water_count > 0) {
		m_water_count--;
		getWorld()->playSound(SOUND_PLAYER_SQUIRT);
		switch (dir) {
		case right:
			x += 4;
			break;
		case left:
			x -= 4;
			break;
		case down:
			y -= 4;
			break;
		case up:
			y += 4;
			break;
		}
		if (getWorld()->isAccessible(x, y, dir)) {
			Actor* s = new Squirt(x, y, dir, getWorld());
			getWorld()->addActor(s);
		}
	}
}

/**
General doSomething function of Frackman
1) Return if dead.
2) Remove all dirt around itself
3) Handle input
*/
void Frackman::doSomething() {
	if (isDead()) return;

	if (getWorld()->removeDirt(getX(), getY(), getX() + 3, getY() + 3)) {
		getWorld()->playSound(SOUND_DIG);
	}

	handleInput();
}

/********** DIRT **********/

/**
Constructor for Dirt
@param int x, int y
*/
Dirt::Dirt(int x, int y, StudentWorld* world)
	: Actor(x, y, true, IID_DIRT, right, 0.25, 3, 1, world)
{};

/**
Dirt's action -
@param int x, int y
*/
void Dirt::doSomething() {
}

/********** BOULDER **********/

/**
Constructor for Boulder
@param int x, int y
*/
Boulder::Boulder(int x, int y, StudentWorld* world)
	: Actor(x, y, true, IID_BOULDER, down, 1.0, 1, BOULDER_HEALTH, world)
	, m_isFalling(false)
	, m_isWaiting(false)
{};

/**
Dirt's action -
- Check if I'm alive
- Am I stable?
*/
void Boulder::doSomething() {
	if (isDead()) return;

	//Check if stable
	if (!m_isWaiting && !m_isFalling) {
		if (!getWorld()->isDirtAt(getX(), getY() - 1, getX() + 3, getY() - 1)) {
			m_isWaiting = true;
			return;
		}
		else
			return;
	}

	//Check if Waiting
	if (m_isWaiting) {
		if (getHealth() == 0) {
			m_isFalling = true;
			m_isWaiting = false;
			getWorld()->playSound(SOUND_FALLING_ROCK);
			return;
		}
		else {
			decrHealthBy(1);
			return;
		}
	}

	//Is Falling
	if (m_isFalling) {
		if (getY() == 0 || getWorld()->isDirtAt(getX(), getY() - 1, getX() + 3, getY() - 1) || getWorld()->hasBlockingActorAt(getX(), getY() - 1, this)) {
			setDead();
		}
		else {
			moveTo(getX(), getY() - 1);
			getWorld()->causeDamageToActorsAt(getX(), getY(), 100, true, ISBOULDER);
		}

	}
}

/********** SQUIRT **********/

/**
Constructor for Squirt
@param int x, int y, int direction
*/
Squirt::Squirt(int x, int y, Direction dir, StudentWorld* world)
	: Actor(x, y, true, IID_WATER_SPURT, dir, 1.0, 1, SQUIRT_HEALTH, world)
{};

/**
Squirt's action -
- Detect collision with actors, check for health
- exit if yes
- Detect collision with walls
- exit?
*/

void Squirt::doSomething() {
	if (getWorld()->causeDamageToActorsAt(getX(), getY(), 2, false, ISSQUIRT) || getHealth() == 0) {
		setDead();
		return;
	}
	else {
		int x = getX(), y = getY();
		switch (getDirection()) {
		case up:
			y++;
			break;
		case down:
			y--;
			break;
		case right:
			x++;
			break;
		case left:
			x--;
			break;
		}
		if (getWorld()->isAccessible(x, y, getDirection())) {
			decrHealthBy(1);
			moveTo(x, y);
		}
		else {
			setDead();
		}
	}
}

///////********** PICK UPS **********/////////

/**
Constructor for Pick Ups
@param int x, int y, int imageID, bool isVisible, bool isPlayerPickup, bool isTemp, StudentWorld* world
*/
Pickups::Pickups(int x, int y, int imageID, bool isVisible, bool isPlayerPickup, bool isTemp, int health, StudentWorld* world)
	: Actor(x, y, isVisible, imageID, right, 1.0, 2, health, world)
	, m_player_pickup(isPlayerPickup)
	, m_protestor_pickup(!isPlayerPickup)
	, m_isTemporary(isTemp)
{};

/********** OIL BARREL **********/

/**
Constructor for Oil
@param int x, int y
*/
Oil::Oil(int x, int y, StudentWorld* world)
	: Pickups(x, y, IID_BARREL, false, true, false, 1, world)
{};

/**
Oil's tick
*/
void Oil::doSomething() {
	if (isDead()) return;
	if (!isVisible() && getWorld()->playerNearMe(this, 4)) {
		setVisible(true);
		return;
	}
	if (getWorld()->playerNearMe(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		return;
	}
}

/********** GOLD NUGGET **********/

/**
Constructor for Gold
@param int x, int y, bool isPlayer
*/
Gold::Gold(int x, int y, bool isPlayer, StudentWorld* world)
	: Pickups(x, y, IID_GOLD, isPlayer, !isPlayer, isPlayer, GOLD_HEALTH, world)
{};

/**
Gold's tick
*/
void Gold::doSomething() {
	if (isDead()) return;
	if (!isVisible() && getWorld()->playerNearMe(this, 4)) {
		setVisible(true);
		return;
	}
	if (canBePickedUpByPlayer() && getWorld()->playerNearMe(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->getFrackman()->increaseGold();
		cerr << "Got gold, total: " << getWorld()->getFrackman()->getGold() << endl;
		return;
	}
	Actor* protestor = nullptr;
	if (canBePickedUpByProtestor() && getWorld()->protestorNearMe(this, protestor, 3)) {
		setDead();
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		getWorld()->increaseScore(25);
		protestor->pickedUpGold();
		cerr << "Protestor picked up gold!" << endl;
		return;
	}
	if (isTemporary()) {
		if (getHealth() == 0) {
			setDead();
			return;
		}
		decrHealthBy(1);
	}

}

/********** SONAR **********/
/**
Constructor for Sonar
@param int x, int y
*/
Sonar::Sonar(int x, int y, StudentWorld* world)
	: Pickups(x, y, IID_SONAR, true, true, true, ((300 - 10 * world->getLevel()) > 100 ? (300 - 10 * world->getLevel()) : 100), world)
{}

/**
Tick handling for Sonar
*/
void Sonar::doSomething() {
	if (isDead()) return;
	if (getWorld()->playerNearMe(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(75);
		getWorld()->getFrackman()->increaseSonar();
		return;
	}
	if (getHealth() == 0) {
		setDead();
		return;
	}
	decrHealthBy(1);
}

/********** WATER **********/
/**
Constructor for Water
@param int x, int y
*/
Water::Water(int x, int y, StudentWorld* world)
	: Pickups(x, y, IID_WATER_POOL, true, true, true, ((300 - 10 * world->getLevel()) > 100 ? (300 - 10 * world->getLevel()) : 100), world)
{}

/**
Tick handling for Water
*/
void Water::doSomething() {
	if (isDead()) return;
	if (getWorld()->playerNearMe(this, 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(100);
		getWorld()->getFrackman()->increaseWater();
		return;
	}
	if (getHealth() == 0) {
		setDead();
		return;
	}
	decrHealthBy(1);
}

///////********** PROTESTORS **********/////////

/**
Constructor for Generic Protestor
@param int x, int y
*/
GenericProtestor::GenericProtestor(int image, int squaresToMove, int health, StudentWorld* world)
	: Actor(60, 60, true, image, left, 1.0, 0, health, world)
	, m_isLeavingField(false)
	, m_ticksToNextMove(0)
	, m_tickDelay(((3 - world->getLevel() / 4) > 0 ? (3 - world->getLevel() / 4) : 0))
	, m_ticksSinceLastShout(15)
	, m_ticksSinceLastPerpendicular(200)
	, m_noSquaresToMoveInCurrentDir(squaresToMove)
{}

/**
Handles the move counter for the protestor
@return true if it's time to move, false otherwise
*/
bool GenericProtestor::shouldIMove() {
	if (m_ticksToNextMove == 0) {
		m_ticksToNextMove = m_tickDelay;
		return true;
	}
	m_ticksToNextMove--;
	return false;
}

void GenericProtestor::doSomething() {
	if (isDead()) return;
	if (!shouldIMove()) return;

	m_ticksSinceLastShout++;
	m_ticksSinceLastPerpendicular++;

	if (isLeaving()) {
		if (getX() == ACTOR_MAXC && getY() == ACTOR_MAXR) {
			setDead();
			return;
		}
		else {
			int nx, ny;
			Direction dir;
			getWorld()->getNextPosition(getX(), getY(), nx, ny, dir);
			setDirection(dir);
			moveTo(nx, ny);
			return;
		}
	}

	if (getWorld()->playerNearMe(this, 4)) {
		if (m_ticksSinceLastShout < 15) return;
		bool faceCorrect = false;
		int p_x = getWorld()->getFrackman()->getX();
		int p_y = getWorld()->getFrackman()->getY();
		switch (getDirection()) {
		case right:
			faceCorrect = p_x > getX();
			break;
		case left:
			faceCorrect = getX() > p_x;
			break;
		case up:
			faceCorrect = getY() < p_y;
			break;
		case down:
			faceCorrect = getY() > p_x;
			break;
		}
		if (faceCorrect) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->getFrackman()->decrHealthBy(2);
			m_ticksSinceLastShout = 0;
			return;
		}
	}

	if (doPathfinding())
		return;

	Direction dir;
	if (getWorld()->isPlayerWithinSight(getX(), getY(), dir)) {
		setDirection(dir);
		switch (dir) {
		case right:
			moveTo(getX() + 1, getY());
			break;
		case left:
			moveTo(getX() - 1, getY());
			break;
		case up:
			moveTo(getX(), getY() + 1);
			break;
		case down:
			moveTo(getX(), getY() - 1);
			break;
		}
		m_noSquaresToMoveInCurrentDir = 0;
		return;
	}

	if (--m_noSquaresToMoveInCurrentDir <= 0) {
		Direction d = randomDirection();
		for (;;) {
			d = randomDirection();
			bool ok = false;
			switch (d) {
			case right:
				ok = getWorld()->isAccessible(getX() + 4, getY(), d);
				break;
			case left:
				ok = getWorld()->isAccessible(getX() - 1, getY(), d);
				break;
			case up:
				ok = getWorld()->isAccessible(getX(), getY() + 4, d);
				break;
			case down:
				ok = getWorld()->isAccessible(getX(), getY() - 1, d);
				break;
			}
			if (ok) break;
			else cerr << "Bad direct " << d << endl;
		}
		cerr << "Setting direction to " << d << endl;
		setDirection(d);
		m_noSquaresToMoveInCurrentDir = randInt(8, 60);
	}


	if (m_ticksSinceLastPerpendicular >= 200) {
		StudentWorld* w = getWorld();
		int x = getX(), y = getY();
		switch (getDirection()) {
		case left:
		case right:
		{
			bool canUp = w->isAccessible(x, y + 4, up);
			bool canDown = w->isAccessible(x, y - 1, down);

			if (canUp && canDown) {
				if (randInt(1, 2) == 1) {
					canUp = false;
				}
				else {
					canDown = false;
				}
			}

			if (canUp) {
				cerr << "SET DIR UP" << endl;
				setDirection(up);
				m_noSquaresToMoveInCurrentDir = randInt(8, 60);
				m_ticksSinceLastPerpendicular = 0;
			}
			else if (canDown) {
				cerr << "SET DIR DOWN" << endl;
				setDirection(down);
				m_noSquaresToMoveInCurrentDir = randInt(8, 60);
				m_ticksSinceLastPerpendicular = 0;
			}
			break;
		}
		case up:
		case down:
		{
			bool canLeft = w->isAccessible(x - 1, y, left);
			bool canRight = w->isAccessible(x + 4, y, right);
			if (canLeft && canRight) {
				if (randInt(1, 2) == 1) {
					canLeft = false;
				}
				else {
					canRight = false;
				}
			}

			if (canLeft) {
				cerr << "SET DIR LEFT" << endl;
				setDirection(left);
				m_noSquaresToMoveInCurrentDir = randInt(8, 60);
				m_ticksSinceLastPerpendicular = 0;
			}
			else if (canRight) {
				cerr << "SET DIR RIGHT" << endl;
				setDirection(right);
				m_noSquaresToMoveInCurrentDir = randInt(8, 60);
				m_ticksSinceLastPerpendicular = 0;
			}
			break;
		}
		}
	}

	Direction d = getDirection();
	switch (d) {
	case left:
		if (getWorld()->isAccessible(getX() - 1, getY(), d)) {
			moveTo(getX() - 1, getY());
			return;
		}
		break;
	case right:
		if (getWorld()->isAccessible(getX() + 4, getY(), d)) {
			moveTo(getX() + 1, getY());
			return;
		}
		break;
	case up:
		if (getWorld()->isAccessible(getX(), getY() + 4, d)) {
			moveTo(getX(), getY() + 1);
			return;
		}
		break;
	case down:
		if (getWorld()->isAccessible(getX(), getY() - 1, d)) {
			moveTo(getX(), getY() - 1);
			return;
		}
		break;
	}

	cerr << "Protestor hit dead end" << endl;
	m_noSquaresToMoveInCurrentDir = 0;
	return;

}

/**
Decreases the Protestor's healthy by n
@return true if the decrement does not empty the health
*/
bool GenericProtestor::decrHealthBy(const int n) {
	if (getHealth() - n > 0) {
		Actor::decrHealthBy(n);
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		m_ticksToNextMove = 50 > (100 - getWorld()->getLevel() * 10) ? 50 : (100 - getWorld()->getLevel() * 10);
		return true;
	}
	else {
		cerr << "Protestor's hp dropped below 0!" << endl;
		Actor::decrHealthBy(n);
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		m_isLeavingField = true;
		m_ticksToNextMove = 0;
		return false;
	}
}

/**
Constructor for regular protestor
*/
RegularProtestor::RegularProtestor(StudentWorld* world)
	: GenericProtestor(IID_PROTESTER, randInt(8, 64), 5, world)
{};

void RegularProtestor::pickedUpGold() {
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	getWorld()->increaseScore(25);
	setLeaving();
}

/** Utility Functions **/
Actor::Direction randomDirection() {
	int r = randInt(1, 4);
	switch (r) {
	case 1:
		return Actor::up;
		break;
	case 2:
		return Actor::down;
		break;
	case 3:
		return Actor::left;
		break;
	case 4:
		return Actor::right;
		break;
	}
	return Actor::up;
}
