#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>

#include "PlayState.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "WindowManager.h"


PlayState::PlayState()
{
}

PlayState::~PlayState()
{
}

void PlayState::entering()
{
	m_actionMap->operator[]("Move_Up") = thor::Action(sf::Keyboard::W, thor::Action::Hold);
	m_actionMap->operator[]("Move_Down") = thor::Action(sf::Keyboard::S, thor::Action::Hold);
	m_actionMap->operator[]("Move_Left") = thor::Action(sf::Keyboard::A, thor::Action::Hold);
	m_actionMap->operator[]("Move_Right") = thor::Action(sf::Keyboard::D, thor::Action::Hold);
	m_exclusive = false;
	std::cout << "Entering play state" << std::endl;
}

void PlayState::leaving()
{
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
	if (getActionMap()->isActive("start_game"))
	{
	}
	printf("HEY");
	return true;
}

void PlayState::draw()
{

}
