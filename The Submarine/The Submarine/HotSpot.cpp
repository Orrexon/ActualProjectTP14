#include "HotSpot.h"


HotSpot::HotSpot()
{
	m_sprite = new sf::Sprite();
}


HotSpot::~HotSpot()
{
	delete m_sprite;
	m_sprite = nullptr;
}


void HotSpot::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(*m_sprite, states);
}
