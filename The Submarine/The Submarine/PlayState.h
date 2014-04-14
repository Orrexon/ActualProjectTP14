#pragma once

#include "GameState.h"
#include "Box2D\Box2D.h"

class Player;
class LevelLoader;
class HotSpot;
class Level;

namespace thor
{
	class UniversalEmitter;
}

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
	void loadNewLevel();
	void createPlayerBodies();

	b2Body* createWall(sf::Vector2f v1, sf::Vector2f v2);
private:
	b2World m_world;
	std::vector<unsigned int> m_mouseIndicies;
	std::vector<Player*> m_players;
	std::vector<b2Body*> m_walls;
	HotSpot* m_hotSpot;
	Level* m_currentLevel;
	LevelLoader* m_levelLoader;

	thor::UniversalEmitter* m_playerParticleEmitter;
};

