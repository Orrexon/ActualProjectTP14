#include <Thor\Input.hpp>
#include <Thor\Particles.hpp>
#include <SFML\Graphics.hpp>
#include <Thor\Math\Random.hpp>
#include <iostream>
#include "PlayState.h"
#include "MenuState.h"
#include "GameState.h"

#include "GameStateManager.h"
#include "WindowManager.h"
#include "ResourceHolder.h"

#include "Player.h"
#include "HotSpot.h"
#include "manymouse.hpp"
#include "LevelLoader.h"
#include "Level.h"
#include "FileSystem.h"
#include "Math.h"
#include "PhysicsHelper.h"
#include "Gatherer.h"
#include "Defender.h"
#include "Box2DWorldDraw.h"
#include "ContactListener.h"
#include "ContactFilter.h"

PlayState::PlayState() : m_world(b2Vec2(0.f, 0.f))
{
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	m_exclusive = false;

	m_contactListener = new ContactListener();
	m_contactFilter = new ContactFilter();

	m_world.SetAllowSleeping(true);
	m_world.SetContactListener(m_contactListener);
	m_world.SetContactFilter(m_contactFilter);

	m_playerParticleEmitter = new thor::UniversalEmitter();
	m_playerParticleEmitter->setEmissionRate(30);
	m_playerParticleEmitter->setParticleLifetime(sf::seconds(5));
	m_playerParticleEmitter->setParticlePosition(thor::Distributions::circle(sf::Vector2f(300, 300), 100));
	m_playerParticleEmitter->setParticleVelocity(thor::Distributions::deflect(sf::Vector2f(1.f, 1.f), 15.f));

	initManyMouse();
	initPlayers();

	m_levelLoader = new LevelLoader();
	m_levelLoader->setDirectory("../levels/");
	m_currentLevel = nullptr;

	loadNewLevel();
	createPlayerBodies();
	m_walls.push_back(createWall(sf::Vector2f(0, 0), sf::Vector2f(1920, 0))); // ⍐
	m_walls.push_back(createWall(sf::Vector2f(1920, 0), sf::Vector2f(1920, 1080))); // ⍈
	m_walls.push_back(createWall(sf::Vector2f(0, 1080), sf::Vector2f(1920, 1080))); // ⍗
	m_walls.push_back(createWall(sf::Vector2f(0, 0), sf::Vector2f(0, 1080))); // ⍇
	
	m_hotSpot = new HotSpot();

	std::cout << "Entering play state" << std::endl;
}

void PlayState::leaving()
{
	auto it = m_players.begin();
	while (it != m_players.end())
	{
		delete *it;
		*it = nullptr;
		++it;
	}
	m_players.clear();

	delete m_playerParticleEmitter;
	m_playerParticleEmitter = nullptr;

	delete m_hotSpot;
	m_hotSpot = nullptr;

	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_contactListener;
	m_contactListener = nullptr;

	delete m_contactFilter;
	m_contactFilter = nullptr;

	std::cout << "Leaving play state" << std::endl;
}

void PlayState::obscuring()
{
}

void PlayState::releaving()
{
}

bool PlayState::update(float dt)
{
	// Fetch raw mouse events
	ManyMouseEvent event;
	while (ManyMouse_PollEvent(&event))
	{
		if (m_players[event.device] == nullptr) continue;
		Player* player = m_players[event.device];

		if (event.type == MANYMOUSE_EVENT_RELMOTION)
		{
			if (event.item == 0) // x
			{
				player->getDefender()->getBody()->ApplyLinearImpulse(b2Vec2(5.f * PhysicsHelper::gameToPhysicsUnits(static_cast<float>(event.value)), 0.f), player->getDefender()->getBody()->GetWorldCenter(), true);
			}
			if (event.item == 1) // y
			{
				player->getDefender()->getBody()->ApplyLinearImpulse(b2Vec2(0.f, 5.f * PhysicsHelper::gameToPhysicsUnits(static_cast<float>(event.value))), player->getDefender()->getBody()->GetWorldCenter(), true);
			}
		}
	}

	m_world.Step(1.f / 60.f, 8, 3);

	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
		player->processEventualDeath();
		player->getDeathTimer()->update();
		player->getParticleSystem()->update(sf::seconds(dt));
		player->getDefender()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getDefender()->getBody()->GetPosition()));
		player->getGatherer()->getSprite()->setPosition(PhysicsHelper::physicsToGameUnits(player->getGatherer()->getBody()->GetPosition()));
	}
	return true;
}

void PlayState::draw()
{
	m_stateAsset->windowManager->getWindow()->draw(*m_currentLevel);
	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
		m_stateAsset->windowManager->getWindow()->draw(*player);
	}

	for (auto &player : m_players)
	{
		if (player == nullptr) continue;
		m_stateAsset->windowManager->getWindow()->draw(*player->getParticleSystem());
	}

	Box2DWorldDraw debugDraw(m_stateAsset->windowManager->getWindow());
	debugDraw.SetFlags(b2Draw::e_shapeBit);
	m_world.SetDebugDraw(&debugDraw);
	m_world.DrawDebugData();
}

void PlayState::initManyMouse()
{
	int numDevices = ManyMouse_Init();
	for (int i = 0; i < numDevices; i++)
	{
		std::string name = ManyMouse_DeviceName(i);
		if (name.find("Pad") != std::string::npos)
		{
			//m_mouseIndicies.push_back(-1);
			m_mouseIndicies.push_back(i);
			continue;
		}
		else
		{
			m_mouseIndicies.push_back(i);
		}
	}
}

void PlayState::initPlayers()
{
	m_players.clear();
	

	for (std::size_t i = 0; i < m_mouseIndicies.size(); i++)
	{
		if (m_mouseIndicies[i] != -1)
		{
			m_players.push_back(new Player());
			
			continue;
		}
		m_players.push_back(nullptr);
	}
}

void PlayState::setupActions()
{
	m_actionMap->operator[]("Move_Up") = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	m_actionMap->operator[]("Move_Down") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	m_actionMap->operator[]("Move_Left") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	m_actionMap->operator[]("Move_Right") = thor::Action(sf::Keyboard::D, thor::Action::Hold);
}

void PlayState::loadNewLevel()
{
	std::vector<std::string> levels = FileSystem::getFilesFromDirectory("../levels/", "level");
	int randomLevelIndex = thor::random(0, static_cast<int>((levels.size() - 1)));

	if (m_currentLevel == nullptr)
	{
		delete m_currentLevel;
		m_currentLevel = nullptr;
	}

	m_currentLevel = m_levelLoader->parseLevel(levels[randomLevelIndex]);
	m_currentLevel->constructObjects(&m_world, m_stateAsset->resourceHolder);
	m_currentLevel->getBackground()->setTexture(m_stateAsset->resourceHolder->getTexture(m_currentLevel->getBackgroundPath(), false));

	m_hotSpot->setRadius(m_currentLevel->getHotspotRadius());
	m_hotSpot->setPosition(m_currentLevel->getHotspotPosition());

	// Create defenders and gatherers
	std::vector<std::string> defender_textures;
	defender_textures.push_back("blue_d.png");
	defender_textures.push_back("red_d.png");
	defender_textures.push_back("yellow_d.png");
	defender_textures.push_back("purple_d.png");

	std::vector<std::string> gatherer_textures;
	gatherer_textures.push_back("blue_g.png");
	gatherer_textures.push_back("red_g.png");
	gatherer_textures.push_back("yellow_g.png");
	gatherer_textures.push_back("purple_g.png");
	for (std::size_t i = 0; i < m_players.size(); i++)
	{
		if (m_players[i] == nullptr) continue;
		m_players[i]->clear();
		Defender* defender = new Defender();
		Gatherer* gatherer = new Gatherer();

		m_stateAsset->resourceHolder->getTexture(defender_textures[i]).setSmooth(true);
		m_stateAsset->resourceHolder->getTexture(gatherer_textures[i]).setSmooth(true);

		defender->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(defender_textures[i]));
		gatherer->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture(gatherer_textures[i]));

		defender->setSpawnPosition(m_currentLevel->getDefenderSpawn(i));
		gatherer->setSpawnPosition(m_currentLevel->getGathererSpawn(i));

		m_players[i]->setDefender(defender);
		m_players[i]->setGatherer(gatherer);

		m_players[i]->initializeParticleSystem(m_stateAsset->resourceHolder);
		
		// Add particle emitters to players particlesystems
		m_players[i]->getParticleSystem()->addEmitter(*m_playerParticleEmitter, sf::seconds(10));
	}
}

void PlayState::createPlayerBodies()
{
	for (auto &player : m_players)
	{
		if (player == nullptr) continue;

		{ // Defender
			b2BodyDef bodyDef;
			b2Vec2 physicsPosition = PhysicsHelper::gameToPhysicsUnits(player->getDefender()->getSprite()->getPosition());
			bodyDef.position.Set(physicsPosition.x, physicsPosition.y);
			bodyDef.type = b2_dynamicBody;
			bodyDef.angle = 0;
			bodyDef.linearDamping = 0.6f;
			bodyDef.angularDamping = 1.f;
			b2Body* body = m_world.CreateBody(&bodyDef);

			b2CircleShape shape;
			shape.m_radius = PhysicsHelper::gameToPhysicsUnits(player->getDefender()->getSprite()->getGlobalBounds().width / 2);

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &shape;
			fixtureDef.restitution = 0.6f;
			fixtureDef.filter.categoryBits = CAT_DEFENDER;
			body->CreateFixture(&fixtureDef);
			player->getDefender()->setBody(body);
		}

		{ // Gatherer
			b2BodyDef bodyDef;
			b2Vec2 physicsPosition = PhysicsHelper::gameToPhysicsUnits(player->getGatherer()->getSprite()->getPosition());
			bodyDef.position.Set(physicsPosition.x, physicsPosition.y);
			bodyDef.type = b2_dynamicBody;
			bodyDef.angle = 0;
			bodyDef.linearDamping = 0.6f;
			b2Body* body = m_world.CreateBody(&bodyDef);

			b2CircleShape shape;
			shape.m_radius = PhysicsHelper::gameToPhysicsUnits(player->getGatherer()->getSprite()->getGlobalBounds().width / 2);

			b2FixtureDef fixtureDef;
			fixtureDef.density = 1;
			fixtureDef.friction = 0.3f;
			fixtureDef.shape = &shape;
			fixtureDef.restitution = 0.6f;
			fixtureDef.filter.categoryBits = CAT_GATHERER;
			body->CreateFixture(&fixtureDef);
			player->getGatherer()->setBody(body);
		}
	}
}

b2Body* PlayState::createWall(sf::Vector2f v1, sf::Vector2f v2)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.angle = 0;

	// Center of line segment
	sf::Vector2f lineCenter((v1.x + v2.x) / 2.f, (v1.y + v2.y) / 2.f);

	// Length of segment
	float length = Math::euclideanDistance(v1, v2);
	
	// Convert the position
	b2Vec2 position = PhysicsHelper::gameToPhysicsUnits(lineCenter);	
	bodyDef.position.Set(position.x, position.y);
	bodyDef.userData = this;
	b2Body* body = m_world.CreateBody(&bodyDef);

	// Create shape and fixture
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.restitution = 0;
	fixtureDef.friction = 1;

	b2EdgeShape edgeShape;
	float physicsLength = PhysicsHelper::gameToPhysicsUnits(length);
	b2Vec2 v1S(-physicsLength / 2.f, 0);
	b2Vec2 v2S(physicsLength / 2.f, 0);
	edgeShape.Set(v1S, v2S);

	fixtureDef.shape = &edgeShape;
	body->CreateFixture(&fixtureDef);
	body->SetTransform(position, Math::angleBetween(v1, v2));
	
	return body;
}