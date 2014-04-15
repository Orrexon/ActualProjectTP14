#pragma once

#include <SFML\Graphics.hpp>
#include <Thor\Time\CallbackTimer.hpp>
#include <Box2D\Dynamics\b2Body.h>

#include "UserData.h"

namespace thor
{
	class ParticleSystem;
}

namespace sf
{
	class Sprite;
}

class ResourceHolder;
class Gatherer;
class Defender;

enum {
	DEFENDER,
	GATHERER
};

class Player;

struct PlayerCharBase
{
	void setType(int type)
	{
		m_type = type;
	}

	bool isType(int type)
	{
		return type == m_type;
	}

	int getType()
	{
		return m_type;
	}

	void setPlayer(Player* player)
	{
		m_player = player;
	}

	bool isSamePlayer(Player* player)
	{
		return player == m_player;
	}

	Player* getPlayer()
	{
		return m_player;
	}

	void setSprite(sf::Sprite* sprite)
	{
		m_sprite = sprite;
	}

	sf::Sprite* getSprite()
	{
		return m_sprite;
	}

	void setSpawnPosition(sf::Vector2f spawn_position)
	{
		m_spawnPosition = spawn_position;
	}

	sf::Vector2f getSpawnPosition()
	{
		return m_spawnPosition;
	}

	void setBody(b2Body* body)
	{
		m_userData = new PlayerUD();
		m_userData->setType(UserDataType::PLAYER);
		m_userData->setData(this);

		m_body = body;
		m_body->SetUserData(m_userData);
	}

	b2Body* getBody()
	{
		return m_body;
	}

	int m_type;
	Player* m_player;
	sf::Sprite* m_sprite;
	sf::Vector2f m_spawnPosition;
	b2Body* m_body;

	PlayerUD* m_userData;
};

class Player: public sf::Drawable
{
public:
	Player();
	~Player();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setDevice(unsigned int p_deviceNo);
	void clear();
	void setDefender(Defender* p_defender);
	void setGatherer(Gatherer* p_gatherer);
	void initializeParticleSystem(ResourceHolder* resourceHolder);
	void processEventualDeath();
	void setDead(bool value);
	thor::CallbackTimer* getDeathTimer();

	Gatherer* getGatherer();
	Defender* getDefender();
	thor::ParticleSystem* getParticleSystem();

	unsigned int getDevice();

	bool isDead();
	void onRespawn(thor::CallbackTimer& trigger);
private:
	bool m_dead;
	bool m_postCheckDead;
	unsigned int m_deviceNo;
	Gatherer* m_gatherer;
	Defender* m_defender;
	thor::ParticleSystem* m_particleSystem;
	thor::CallbackTimer* m_deathTimer;
};

