#pragma once

#include <SFML\Graphics.hpp>

class Gatherer;
class Defender;

enum PlayerEnum
{
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
	PLAYER_4
};

class Player: public sf::Drawable
{
public:
	Player(PlayerEnum p_identifier);
	~Player();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setDevice(unsigned int p_deviceNo);

	Gatherer* getGatherer();
	Defender* getDefender();

private:
	unsigned int m_deviceNo;
	PlayerEnum m_identifier;
	Gatherer* m_gatherer;
	Defender* m_defender;
};

