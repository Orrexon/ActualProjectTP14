#pragma once

#include <Box2D\Dynamics\b2Body.h>

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
	void setBody(b2Body* body);
	b2Body* getBody();

private:
	sf::Sprite* m_sprite;
	b2Body* m_body;
};

