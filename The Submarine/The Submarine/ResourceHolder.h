#pragma once

#include <string>
#include <Thor\Resources.hpp>
#include <SFML\Graphics\Texture.hpp>
#include <SFML\Graphics\Font.hpp>
#include <SFML\Audio\SoundBuffer.hpp>

class ResourceHolder
{
public:
	ResourceHolder();

	sf::Texture& getTexture(const std::string& p_filename);
	sf::Font& getFont(const std::string& p_filename);
	sf::SoundBuffer& getSound(const std::string& p_filename);

private:
	std::string m_texturePath;
	std::string m_fontPath;
	std::string m_soundPath;

	thor::MultiResourceCache m_cache;
};

