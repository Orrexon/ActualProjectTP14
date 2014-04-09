#include "ResourceHolder.h"

ResourceHolder::ResourceHolder()
{
	m_cache.setReleaseStrategy(thor::Resources::ExplicitRelease);
	m_cache.setLoadingFailureStrategy(thor::Resources::ReturnNullPointer);

	m_texturePath = "../assets/textures/";
	m_fontPath = "../assets/fonts/";
	m_soundPath = "../assets/sounds/";
}

sf::Texture& ResourceHolder::getTexture(const std::string& p_filename)
{
	std::string path(m_texturePath + p_filename);
	return *m_cache.acquire(thor::Resources::fromFile<sf::Texture>(path));
}
sf::Font& ResourceHolder::getFont(const std::string& p_filename)
{
	std::string path(m_fontPath + p_filename);
	return *m_cache.acquire(thor::Resources::fromFile<sf::Font>(path));
}
sf::SoundBuffer& ResourceHolder::getSound(const std::string& p_filename)
{
	std::string path(m_soundPath + p_filename);
	return *m_cache.acquire(thor::Resources::fromFile<sf::SoundBuffer>(path));
}

