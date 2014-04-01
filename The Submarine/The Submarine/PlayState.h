#pragma once

#include "GameState.h"

class Player;
class HotSpot;

class PlayState : public GameState
{
public:
	PlayState();
	~PlayState();

	void entering();
	void leaving();
	void obscuring();
	void releaving();
	bool update(float dt);
	void draw();
	void setupActions();

	void initManyMouse();
	void initPlayers();
private:
	uint8_t m_activePlayers;
	std::vector<Player*> m_players;
	HotSpot* m_hotSpot;
};

