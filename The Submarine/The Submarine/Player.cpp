#include "Player.h"
#include "Gatherer.h"
#include "Defender.h"
#include <iostream>

Player::Player(PlayerEnum p_identifier)
{
	m_identifier = p_identifier;
	m_defender = nullptr;
	m_gatherer = nullptr;
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
	target.draw(*m_gatherer->getSprite());
	target.draw(*m_defender->getSprite());
}

void Player::setDevice(unsigned int p_deviceNo)
{
	m_deviceNo = p_deviceNo;
}

void Player::clear()
{
	if (m_defender != nullptr)
	{
		delete m_defender;
		m_defender = nullptr;
	}

	if (m_gatherer != nullptr)
	{
		delete m_gatherer;
		m_gatherer = nullptr;
	}
}
void Player::setDefender(Defender* p_defender)
{
	m_defender = p_defender;
}
void Player::setGatherer(Gatherer* p_gatherer)
{
	m_gatherer = p_gatherer;
}

Gatherer* Player::getGatherer()
{
	return m_gatherer;
}

Defender* Player::getDefender()
{
	return m_defender;
}
