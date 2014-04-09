#pragma once

namespace sf
{
	class Sprite;
}

class Defender
{
public:
	Defender();
	~Defender();

	sf::Sprite* getSprite();

private:
	sf::Sprite* m_sprite;
};

