#include "HotSpot.h"


HotSpot::HotSpot()
{
}


HotSpot::~HotSpot()
{
}

void HotSpot::setRadius(float radius)
{
	m_radius = radius;
}
void HotSpot::setPosition(sf::Vector2f position)
{
	m_position = position;
}
float HotSpot::getRadius()
{
	return m_radius;
}
sf::Vector2f HotSpot::getPosition()
{
	return m_position;
}