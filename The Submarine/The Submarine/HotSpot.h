#pragma once

#include <SFML\Graphics.hpp>

class HotSpot
{
public:
	HotSpot();
	~HotSpot();

	void setRadius(float radius);
	void setPosition(sf::Vector2f position);

	float getRadius();
	sf::Vector2f getPosition();

private:
	float m_radius;
	sf::Vector2f m_position;
};

