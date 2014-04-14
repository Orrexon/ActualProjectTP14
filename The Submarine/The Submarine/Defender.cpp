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

void Defender::setBody(b2Body* body)
{
	m_body = body;
}

b2Body* Defender::getBody()
{
	return m_body;
}