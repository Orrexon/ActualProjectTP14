#pragma once

#include <SFML\Graphics.hpp>

namespace thor
{
	class ParticleSystem;
}

class ResourceHolder;
class Gatherer;
class Defender;

class Player: public sf::Drawable
{
public:
	Player();
	~Player();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setDevice(unsigned int p_deviceNo);
	void clear();
	void setDefender(Defender* p_defender);
	void setGatherer(Gatherer* p_gatherer);
	void initializeParticleSystem(ResourceHolder* resourceHolder);

	Gatherer* getGatherer();
	Defender* getDefender();
	thor::ParticleSystem* getParticleSystem();

	unsigned int getDevice();

private:
	unsigned int m_deviceNo;
	Gatherer* m_gatherer;
	Defender* m_defender;
	thor::ParticleSystem* m_particleSystem;
};

