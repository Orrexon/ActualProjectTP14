#include "Defender.h"
#include <SFML\Graphics\Sprite.hpp>

Defender::Defender()
{
	m_sprite = new sf::Sprite();
}

Defender::~Defender()
{
	delete m_sprite;
	m_sprite = nullptr;
}

sf::Sprite* Defender::getSprite()
{
	return m_sprite;
}
