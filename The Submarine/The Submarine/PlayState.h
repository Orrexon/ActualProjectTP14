#pragma once

#include "GameState.h"

class Player;
class HotSpot;

class PlayState : public GameState
{
public:
	PlayState();
	~PlayState();

	virtual void entering();
	virtual void leaving();
	virtual void obscuring();
	virtual void releaving();
	virtual bool update(float dt);
	virtual void draw();

	void initManyMouse();
private:
	std::vector<Player*> m_players;
	HotSpot* m_hotSpot;
};

