#include "Gatherer.h"
#include <SFML\Graphics\Sprite.hpp>

Gatherer::Gatherer()
{
	m_sprite = new sf::Sprite();
}

Gatherer::~Gatherer()
{
	delete m_sprite;
	m_sprite = nullptr;

	delete m_userData;
	m_userData = nullptr;
}