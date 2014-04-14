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

	delete m_userData;
	m_userData = nullptr;
}