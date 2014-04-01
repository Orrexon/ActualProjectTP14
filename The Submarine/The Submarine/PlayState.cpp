#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>

#include "PlayState.h"
#include "MenuState.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "WindowManager.h"
#include "Player.h"
#include "HotSpot.h"
#include "manymouse.hpp"

PlayState::PlayState()
{
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	m_activePlayers = 0;
	m_exclusive = false;
	
	initManyMouse();
	initPlayers();

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