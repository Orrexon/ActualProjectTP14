#pragma once

namespace sf
{
	class Sprite;
}

class Gatherer
{
public:
	Gatherer();
	~Gatherer();

	sf::Sprite* getSprite();

private:
	sf::Sprite* m_sprite;
};

