#pragma once

#include <SFML\Graphics.hpp>

class HotSpot: public sf::Drawable
{
public:
	HotSpot();
	~HotSpot();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:
	sf::Sprite* m_sprite;
};

