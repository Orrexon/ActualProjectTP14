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
}

sf::Sprite* Gatherer::getSprite()
{
	return m_sprite;
}
