#pragma once

#include "GameState.h"
#include <vector>
#include <json\value.h>
#include <SFML\Graphics.hpp>

namespace sf
{
	class Texture;
	class View;
	class CircleShape;
}

struct PlayerStruct
{
	sf::CircleShape* defender;
	sf::CircleShape* gatherer;
};

struct Object
{
	std::string image_path;
	std::string object_path;
	sf::Sprite* sprite;
	float circle_radius;
	bool use_circle_collision;
	sf::Vector2f circle_position;
	std::vector<sf::Vector2f> points;
};


class LevelEditorState : public GameState
{
public:
	LevelEditorState();
	~LevelEditorState();

	void entering();
	void leaving();
	void obscuring();
	void releaving();
	bool update(float dt);
	void draw();
	void setupActions();

	void openFile();
	void saveFile();
	void newFile();
	void exitFile();
	void changeBackground();
	void handlePlayer(int player_id, int char_id);
	void chooseHotspotImage();
	void setHotspotRadius();
	void setDefault();
	void loadObject();
	void changeHotspotImage();
	void loadObjectRaw(std::string objectFile);
	void handleMenuSelected(thor::ActionContext<std::string> context);

private:
	Json::Value m_jsonRoot;
	thor::ActionMap<std::string>::CallbackSystem* m_system;

	std::string m_backgroundPath;
	std::string m_hotspotPath;

	// SPRITE & TEXTURES
	sf::Texture* m_backgroundTexture;

	sf::Sprite* m_hotspotSprite;
	sf::Sprite* m_backgroundSprite;
	sf::CircleShape m_hotSpotShape;

	sf::View* m_levelView;
	sf::Vector2i m_scrollHandlePosition;
	sf::Vector2f m_defaultCenter;

	// CREATE OBJECT THINGS
	std::vector<Object> objects;
	
	
	bool m_exit;
	bool m_scrollingScreen;

	std::vector<PlayerStruct*> m_players;
	bool m_placingPlayer;
	bool m_placingHotspot;
	bool m_placingObject;
	
	int m_placingPlayerIndex;
	int m_placingPlayerChartype;
	int m_placingObjectIndex;

	HWND m_dialogEditHotspot;
};