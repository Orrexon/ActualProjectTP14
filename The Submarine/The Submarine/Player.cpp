#include "Player.h"
#include "Gatherer.h"
#include "Defender.h"
#include <iostream>

Player::Player(PlayerEnum p_identifier)
{
	m_identifier = p_identifier;
	m_gatherer = new Gatherer();
	m_defender = new Defender();
}


Player::~Player()
{
	delete m_gatherer;
	m_gatherer = nullptr;

	delete m_defender;
	m_defender = nullptr;
}


void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	std::cout << "BAJS" << std::endl;
}

void Player::setDevice(unsigned int p_deviceNo)
{
	m_deviceNo = p_deviceNo;
}


Gatherer* Player::getGatherer()
{
	return m_gatherer;
}

Defender* Player::getDefender()
{
	return m_defender;
}
