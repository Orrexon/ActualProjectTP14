#include <Thor/Particles.hpp>

#include "Player.h"
#include "Gatherer.h"
#include "Defender.h"
#include "ResourceHolder.h"
#include "Box2D\Dynamics\b2Body.h"
#include "PhysicsHelper.h"

#include <iostream>

Player::Player()
{
	m_postCheckDead = false;
	m_defender = nullptr;
	m_gatherer = nullptr;
	m_particleSystem = new thor::ParticleSystem();

	m_deathTimer = new thor::CallbackTimer();
	m_deathTimer->connect(std::bind(&Player::onRespawn, this, std::placeholders::_1));
}

Player::~Player()
{
	delete m_gatherer;
	m_gatherer = nullptr;

	delete m_defender;
	m_defender = nullptr;

	delete m_particleSystem;
	m_particleSystem = nullptr;

	delete m_deathTimer;
	m_deathTimer = nullptr;
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	if (!m_dead)
	{
		target.draw(*m_gatherer->getSprite());
		target.draw(*m_defender->getSprite());
	}
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

	m_dead = false;
}
void Player::setDefender(Defender* p_defender)
{
	m_defender = p_defender;
	m_defender->setPlayer(this);
	m_defender->setType(DEFENDER);
	m_defender->getSprite()->setPosition(m_defender->getSpawnPosition());
	m_defender->getSprite()->setOrigin(m_defender->getSprite()->getLocalBounds().width / 2, m_defender->getSprite()->getLocalBounds().height / 2);

	
}
void Player::setGatherer(Gatherer* p_gatherer)
{
	m_gatherer = p_gatherer;
	m_gatherer->setPlayer(this);
	m_gatherer->setType(GATHERER);
	m_gatherer->getSprite()->setPosition(m_gatherer->getSpawnPosition());
	m_gatherer->getSprite()->setOrigin(m_gatherer->getSprite()->getLocalBounds().width / 2, m_gatherer->getSprite()->getLocalBounds().height / 2);
}
void Player::initializeParticleSystem(ResourceHolder* resourceHolder)
{
	m_particleSystem->setTexture(resourceHolder->getTexture("particle_round.png"));
}
void Player::setDead(bool value)
{
	m_dead = value;
	m_postCheckDead = true;
}
void Player::processEventualDeath()
{
	if (!m_postCheckDead) return;
	if (m_dead)
	{
		m_gatherer->getBody()->SetActive(false);
		m_gatherer->getBody()->SetTransform(PhysicsHelper::gameToPhysicsUnits(m_gatherer->getSpawnPosition()), m_gatherer->getBody()->GetAngle());
		m_gatherer->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
		m_gatherer->getBody()->SetAngularVelocity(0.f);

		m_defender->getBody()->SetActive(false);
		m_defender->getBody()->SetTransform(PhysicsHelper::gameToPhysicsUnits(m_defender->getSpawnPosition()), m_defender->getBody()->GetAngle());
		m_defender->getBody()->SetLinearVelocity(b2Vec2(0.f, 0.f));
		m_defender->getBody()->SetAngularVelocity(0.f);

		m_deathTimer->restart(sf::seconds(3.f));
	}
	else
	{
		m_defender->getBody()->SetActive(true);
		m_gatherer->getBody()->SetActive(true);
	}
	m_postCheckDead = false;
}
thor::CallbackTimer* Player::getDeathTimer()
{
	return m_deathTimer;
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
bool Player::isDead()
{
	return m_dead;
}
void Player::onRespawn(thor::CallbackTimer& trigger)
{
	setDead(false);
}
