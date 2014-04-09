#pragma once

#include "GameState.h"
#include <vector>
#include <json\value.h>

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
	void handleMenuSelected(thor::ActionContext<std::string> context);

private:
	Json::Value m_jsonRoot;
	thor::ActionMap<std::string>::CallbackSystem* m_system;

	std::string m_backgroundPath;
	std::string m_hotspotPath;

	// SPRITE & TEXTURES
	sf::Texture* m_backgroundTexture;
	sf::Texture* m_hotSpotTexture;

	sf::Sprite* m_backgroundSprite;
	sf::Sprite* m_hotSpotSprite;

	sf::View* m_levelView;
	sf::Vector2i m_scrollHandlePosition;
	sf::Vector2f m_defaultCenter;
	
	bool m_exit;
	bool m_scrollingScreen;

	std::vector<PlayerStruct*> m_players;
	bool m_placingPlayer;
	bool m_placingHotspot;
	int m_placingPlayerIndex;
	int m_placingPlayerChartype;

	HWND m_dialogEditHotspot;
};