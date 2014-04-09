#include "Level.h"

Level::Level()
{
	m_background = new sf::Sprite();
	m_defenderSpawn.resize(4);
	m_gathererSpawn.resize(4);
}

Level::~Level()
{
}

void Level::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(*m_background);
}

void Level::setBackgroundPath(std::string p_filepath)
{
	m_backgroundPath = p_filepath;
}
std::string Level::getBackgroundPath()
{
	return m_backgroundPath;
}

void Level::setDefenderSpawn(int player_index, float x, float y)
{
	sf::Vector2f spawn(x, y);
	m_defenderSpawn[player_index] = spawn;
}
void Level::setGathererSpawn(int player_index, float x, float y)
{
	sf::Vector2f spawn(x, y);
	m_gathererSpawn[player_index] = spawn;
}
sf::Vector2f Level::getDefenderSpawn(int player_index)
{
	return m_defenderSpawn[player_index];
}
sf::Vector2f Level::getGathererSpawn(int player_index)
{
	return m_gathererSpawn[player_index];
}