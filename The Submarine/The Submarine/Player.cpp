#include <Thor/Particles.hpp>

#include "Player.h"
#include "Gatherer.h"
#include "Defender.h"
#include "ResourceHolder.h"

#include <iostream>

Player::Player()
{
	m_defender = nullptr;
	m_gatherer = nullptr;
	m_particleSystem = new thor::ParticleSystem();
}

Player::~Player()
{
	delete m_gatherer;
	m_gatherer = nullptr;

	delete m_defender;
	m_defender = nullptr;

	delete m_particleSystem;
	m_particleSystem = nullptr;
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
void Player::initializeParticleSystem(ResourceHolder* resourceHolder)
{
	m_particleSystem->setTexture(resourceHolder->getTexture("particle_round.png"));
}

thor::ParticleSystem* Player::getParticleSystem()
{
	return m_particleSystem;
}
Gatherer* Player::getGatherer()
{
	return m_gatherer;
}

Defender* Player::getDefender()
{
	return m_defender;
}
unsigned int Player::getDevice()
{
	return m_deviceNo;
}

