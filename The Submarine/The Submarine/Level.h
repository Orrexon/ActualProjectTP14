#pragma once

#include <SFML\System\Vector2.hpp>
#include <Box2D\Dynamics\b2Body.h>
#include <Thor\Math.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <vector>

namespace sf
{
	class Sprite;
}

struct LevelObject
{
	bool use_circle;
	float circle_radius;
	std::string image_path;
	sf::Sprite sprite;
	b2Body* body;
	std::vector<sf::Vector2f> vertices;
	std::vector<thor::Triangle<sf::Vector2f>> triangles;
};

class ResourceHolder;

class Level: public sf::Drawable
{
public:
	Level();
	~Level();

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	void addObject(LevelObject* obj);
	void setBackgroundPath(std::string p_filepath);
	void setDefenderSpawn(int player_index, float x, float y);
	void setGathererSpawn(int player_index, float x, float y);
	void constructObjects(b2World* world, ResourceHolder* resourceHolder);
	

	sf::Vector2f getDefenderSpawn(int player_index);
	sf::Vector2f getGathererSpawn(int player_index);
	sf::Sprite* getBackground();
	std::string getBackgroundPath();
private:
	sf::Sprite* m_background;
	std::string m_backgroundPath;
	std::vector<sf::Vector2f> m_defenderSpawn;
	std::vector<sf::Vector2f> m_gathererSpawn;
	std::vector<LevelObject*> m_objects;
};

