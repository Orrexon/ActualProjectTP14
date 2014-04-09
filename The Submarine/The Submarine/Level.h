#pragma once

#include <SFML\System\Vector2.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <vector>

namespace sf
{
	class Sprite;
}

class Level: public sf::Drawable
{
public:
	Level();
	~Level();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void setBackgroundPath(std::string p_filepath);
	std::string getBackgroundPath();

	void setDefenderSpawn(int player_index, float x, float y);
	void setGathererSpawn(int player_index, float x, float y);

	sf::Vector2f getDefenderSpawn(int player_index);
	sf::Vector2f getGathererSpawn(int player_index);
private:
	sf::Sprite* m_background;
	std::string m_backgroundPath;
	std::vector<sf::Vector2f> m_defenderSpawn;
	std::vector<sf::Vector2f> m_gathererSpawn;
};

