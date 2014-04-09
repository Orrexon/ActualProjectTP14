#include <Thor\Input.hpp>
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
#include "Gatherer.h"
#include "Defender.h"

PlayState::PlayState()
{
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	initPlayers();
	initManyMouse();

	m_levelLoader = new LevelLoader();
	m_levelLoader->setDirectory("../levels/");
	m_currentLevel = nullptr;
	loadNewLevel();

	m_activePlayers = 0;
	m_exclusive = false;
	

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

	delete m_hotSpot;
	m_hotSpot = nullptr;

	delete m_actionMap;
	m_actionMap = nullptr;
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
	if (getActionMap()->isActive("Move_Down"))
	{
		m_stateAsset->gameStateManager->changeState(new MenuState());
	}
	return true;
}

void PlayState::draw()
{
	m_stateAsset->windowManager->getWindow()->draw(*m_currentLevel);
	for (auto &player : m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player);
	}
	m_stateAsset->windowManager->getWindow()->draw(*m_hotSpot);
}

void PlayState::initManyMouse()
{
	int numDevices = ManyMouse_Init();
	std::cout << "Following mouse devices was found on driver " << ManyMouse_DriverName << std::endl;

	for (int i = 0; i < numDevices; i++)
	{
		std::string name = ManyMouse_DeviceName(i);
		if (name.find("Pad") != std::string::npos)
		{
			m_activePlayers++;
			std::cout << "Maybe a touchpad was found. I am ignoring it for you" << std::endl;
			continue;
		}
	}
	std::cout << "Active mouse devices " << m_activePlayers << std::endl;
}

void PlayState::initPlayers()
{
	m_players.push_back(new Player(PLAYER_1));
	m_players.push_back(new Player(PLAYER_2));
	m_players.push_back(new Player(PLAYER_3));
	m_players.push_back(new Player(PLAYER_4));
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

	// Create defenders and gatherers
	for (int i = 0; i < m_players.size(); i++)
	{
		m_players[i]->clear();
		Defender* defender = new Defender();
		Gatherer* gatherer = new Gatherer();

		defender->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("defender.png"));
		gatherer->getSprite()->setTexture(m_stateAsset->resourceHolder->getTexture("gatherer.png"));

		defender->getSprite()->setPosition(m_currentLevel->getDefenderSpawn(i));
		gatherer->getSprite()->setPosition(m_currentLevel->getGathererSpawn(i));
		
		m_players[i]->setDefender(defender);
		m_players[i]->setGatherer(gatherer);
	}
}