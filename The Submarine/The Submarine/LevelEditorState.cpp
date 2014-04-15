#pragma comment(lib, "Shlwapi.lib")

#include <Thor\Input.hpp>
#include <Thor\Math.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <json\json.h>
#include <fstream>
#include <Shlwapi.h>
#include <fstream>
#include <regex>

#include "GameStateAssets.h"
#include "LevelEditorState.h"
#include "resource.h"
#include "Math.h"
#include "String.h"
#include "FileSystem.h"

enum
{
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
	ID_DEFAULT,
	ID_FILE_EXIT,
	ID_CREATE_OBJECT,
	ID_LOAD_OBJECT,
	ID_BACKGROUND_CHANGE,
	ID_PLAYER_1,
	ID_PLAYER_2,
	ID_PLAYER_3,
	ID_PLAYER_4,
	ID_PLAYER_1_GATHERER,
	ID_PLAYER_1_DEFENDER,
	ID_PLAYER_2_GATHERER,
	ID_PLAYER_2_DEFENDER,
	ID_PLAYER_3_GATHERER,
	ID_PLAYER_3_DEFENDER,
	ID_PLAYER_4_GATHERER,
	ID_PLAYER_4_DEFENDER,
	ID_HOTSPOT_IMAGE,
	ID_HOTSPOT_RADIUS,
	ID_OBJECT_LOAD
};

enum
{
	PLAYER_DEFENDER,
	PLAYER_GATHERER
};

enum
{
	PLAYER_1,
	PLAYER_2,
	PLAYER_3,
	PLAYER_4
};

const std::string DEFAULT_BACKGROUND = "../assets/textures/backgrounds/default.png";
const int DEFENDER_RADIUS = 30;
const int GATHERER_RADIUS = 15;
BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lparam);

LevelEditorState::LevelEditorState()
{

}

LevelEditorState::~LevelEditorState()
{

}

void LevelEditorState::entering()
{
	sf::Vector2u windowSize = m_stateAsset->windowManager->getWindow()->getSize();
	m_dialogEditHotspot = nullptr;

	m_placingHotspot = false;
	m_placingPlayer = false;
	m_exit = false;
	m_scrollingScreen = false;

	m_placingPlayerIndex = 0;
	m_placingPlayerChartype = 0;
	
	std::vector<sf::Color> colors;
	colors.push_back(sf::Color::Red);
	colors.push_back(sf::Color::Green);
	colors.push_back(sf::Color::Yellow);
	colors.push_back(sf::Color::Blue);

	std::vector<sf::Vector2f> defender_positions;
	defender_positions.push_back(sf::Vector2f(100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, windowSize.y - 100));
	defender_positions.push_back(sf::Vector2f(100, windowSize.y - 100));

	std::vector<sf::Vector2f> gatherer_positions;
	gatherer_positions.push_back(sf::Vector2f(50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, windowSize.y - 50));
	gatherer_positions.push_back(sf::Vector2f(50, windowSize.y - 50));

	for (int i = 0; i < 4; i++)
	{
		PlayerStruct* player = new PlayerStruct();
		player->defender = new sf::CircleShape();
		player->defender->setFillColor(colors[i]);
		player->defender->setPosition(defender_positions[i]);
		player->defender->setOrigin(DEFENDER_RADIUS, DEFENDER_RADIUS);
		player->defender->setRadius(DEFENDER_RADIUS);

		player->gatherer = new sf::CircleShape();
		player->gatherer->setFillColor(colors[i]);
		player->gatherer->setPosition(gatherer_positions[i]);
		player->gatherer->setOrigin(GATHERER_RADIUS, GATHERER_RADIUS);
		player->gatherer->setRadius(GATHERER_RADIUS);

		m_players.push_back(player);
	}

	m_backgroundPath = DEFAULT_BACKGROUND;
	m_backgroundSprite = new sf::Sprite();
	m_backgroundTexture = new sf::Texture();
	m_backgroundTexture->loadFromFile(DEFAULT_BACKGROUND);
	m_backgroundSprite->setTexture(*m_backgroundTexture);

	m_hotSpotShape.setRadius(100);
	m_hotSpotShape.setOrigin(m_hotSpotShape.getRadius(), m_hotSpotShape.getRadius());
	m_hotSpotShape.setPosition(m_backgroundSprite->getGlobalBounds().width / 2.f, m_backgroundSprite->getGlobalBounds().height / 2.f);
	m_hotSpotShape.setFillColor(sf::Color(100, 160, 100, 255));

	m_levelView = new sf::View();
	m_levelView->setSize(sf::Vector2f(windowSize.x, windowSize.y));
	m_levelView->setCenter(windowSize.x / 2, windowSize.y / 2);
	
	m_jsonRoot = new Json::Value();
	m_exclusive = false;

	std::cout << "Entering level editor state" << std::endl;

	HMENU menuBar = CreateMenu();
	HMENU hFileDropDown = CreateMenu();
	HMENU hObjectDropDown = CreateMenu();
	HMENU hBackgroundDropDown = CreateMenu();
	HMENU hPlayerDropDown = CreatePopupMenu();
	HMENU hHotSpotDropDown = CreateMenu();
	HMENU hPowerupsDropDown = CreateMenu();
	HMENU hPlayer1Sub = CreatePopupMenu();
	HMENU hPlayer2Sub = CreatePopupMenu();
	HMENU hPlayer3Sub = CreatePopupMenu();
	HMENU hPlayer4Sub = CreatePopupMenu();


	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hFileDropDown, "File");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hObjectDropDown, "Objects");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hBackgroundDropDown, "Background");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hPlayerDropDown, "Player");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hHotSpotDropDown, "HotSpot");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hPowerupsDropDown, "Powerups");

	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer1Sub, "Player 1");
	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer2Sub, "Player 2");
	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer3Sub, "Player 3");
	AppendMenuA(hPlayerDropDown, MF_POPUP, (UINT_PTR)hPlayer4Sub, "Player 4");

	AppendMenuA(hPlayer1Sub, MF_STRING | MF_POPUP, ID_PLAYER_1_GATHERER, "Gatherer");
	AppendMenuA(hPlayer1Sub, MF_STRING | MF_POPUP, ID_PLAYER_1_DEFENDER, "Defender");
	AppendMenuA(hPlayer2Sub, MF_STRING | MF_POPUP, ID_PLAYER_2_GATHERER, "Gatherer");
	AppendMenuA(hPlayer2Sub, MF_STRING | MF_POPUP, ID_PLAYER_2_DEFENDER, "Defender");
	AppendMenuA(hPlayer3Sub, MF_STRING | MF_POPUP, ID_PLAYER_3_GATHERER, "Gatherer");
	AppendMenuA(hPlayer3Sub, MF_STRING | MF_POPUP, ID_PLAYER_3_DEFENDER, "Defender");
	AppendMenuA(hPlayer4Sub, MF_STRING | MF_POPUP, ID_PLAYER_4_GATHERER, "Gatherer");
	AppendMenuA(hPlayer4Sub, MF_STRING | MF_POPUP, ID_PLAYER_4_DEFENDER, "Defender");

	AppendMenuA(hHotSpotDropDown, MF_STRING, ID_HOTSPOT_IMAGE, "Choose image");
	AppendMenuA(hHotSpotDropDown, MF_STRING, ID_HOTSPOT_RADIUS, "Set radius");
	
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_NEW, "New level");
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_OPEN, "Open level");
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_SAVE, "Save level");
	AppendMenuA(hFileDropDown, MF_STRING, ID_DEFAULT, "Set default\tCTRL+D");
	AppendMenuA(hFileDropDown, MF_SEPARATOR | MF_BYPOSITION, 0, nullptr);
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_EXIT, "Exit");

	AppendMenuA(hObjectDropDown, MF_STRING, ID_LOAD_OBJECT, "Load object");

	AppendMenuA(hBackgroundDropDown, MF_STRING, ID_BACKGROUND_CHANGE, "Change background");

	HBITMAP p1_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP1));
	HBITMAP p2_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP2));
	HBITMAP p3_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP3));
	HBITMAP p4_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP4));
	
	DWORD dLastError = GetLastError();
	LPCTSTR strErrorMessage = NULL;

	SetMenuItemBitmaps(hPlayerDropDown, 0, MF_BYPOSITION, p1_icon, p1_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 1, MF_BYPOSITION, p2_icon, p2_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 2, MF_BYPOSITION, p3_icon, p3_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 3, MF_BYPOSITION, p4_icon, p4_icon);

	RECT rect;
	GetClientRect(m_stateAsset->windowManager->getWindow()->getSystemHandle(), &rect);
	
	m_levelView->setSize(sf::Vector2f(1920, 1055));
	
	m_stateAsset->windowManager->setMenu(menuBar);

	m_system = new thor::ActionMap<std::string>::CallbackSystem;
	m_system->connect("MenuSelected", std::bind(&LevelEditorState::handleMenuSelected, this, std::placeholders::_1));
}

void LevelEditorState::leaving()
{
	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_system;
	m_system = nullptr;

	delete m_backgroundSprite;
	m_backgroundSprite = nullptr;

	delete m_backgroundTexture;
	m_backgroundTexture = nullptr;

	delete m_levelView;
	m_levelView = nullptr;

	std::cout << "Leaving level editor state" << std::endl;
}

void LevelEditorState::obscuring()
{
}

void LevelEditorState::releaving()
{
}

bool LevelEditorState::update(float dt)
{
	sf::Vector2f current_mouse_pos = m_stateAsset->windowManager->getWindow()->mapPixelToCoords(sf::Mouse::getPosition(*m_stateAsset->windowManager->getWindow()));
	sf::Vector2i local_mouse_pos = sf::Mouse::getPosition(*m_stateAsset->windowManager->getWindow());
	getActionMap()->invokeCallbacks(*m_system, nullptr);
	
	if (m_exit)
	{
		return false;
	}

	if (getActionMap()->isActive("Drag"))
	{
		if (!m_scrollingScreen)
		{
			m_scrollHandlePosition = local_mouse_pos;
			m_scrollingScreen = !m_scrollingScreen;
			m_defaultCenter = m_levelView->getCenter();
		}
	}
	else
	{
		m_scrollingScreen = false;
	}

	if (m_scrollingScreen)
	{
		// Get direction of draggin
		float radians = std::atan2f(local_mouse_pos.y - m_scrollHandlePosition.y, local_mouse_pos.x - m_scrollHandlePosition.x);
		float dir_x_r = std::cosf(radians);
		float dir_y_r = std::sinf(radians);


		// Get draggin distance
		float distance = std::sqrtf(
			(local_mouse_pos.x - m_scrollHandlePosition.x) * (local_mouse_pos.x - m_scrollHandlePosition.x) +
			(local_mouse_pos.y - m_scrollHandlePosition.y) * (local_mouse_pos.y - m_scrollHandlePosition.y));
		
		m_levelView->setCenter(m_defaultCenter + sf::Vector2f(distance * -dir_x_r, distance * -dir_y_r));

		// Cap the view to not cross borders of the background
		sf::Vector2f max(m_backgroundSprite->getGlobalBounds().width, m_backgroundSprite->getGlobalBounds().height);
		sf::Vector2f center = m_levelView->getCenter();
		sf::Vector2f size = m_levelView->getSize();
		sf::Vector2f newCenter = center;
		if (center.x - size.x / 2 <= 0)
		{
			newCenter.x = size.x / 2;
		}
		else if (center.x + size.x / 2 >= max.x)
		{
			newCenter.x = max.x - size.x / 2;
		}
		if (center.y - size.y / 2 <= 0)
		{
			newCenter.y = size.y / 2;
		}
		else if (center.y + size.y / 2 >= max.y)
		{
			newCenter.y = max.y - size.y / 2;
		}
		m_levelView->setCenter(newCenter);
	}

	// Changing hotspot position
	if (m_placingHotspot)
	{
		if (getActionMap()->isActive("Center"))
		{
			m_hotSpotShape.setPosition(m_backgroundSprite->getGlobalBounds().width / 2.f, m_backgroundSprite->getGlobalBounds().height / 2.f);
		}
		else
		{
			m_hotSpotShape.setPosition(current_mouse_pos);
		}

		sf::Color color = m_hotSpotShape.getFillColor();
		color.a = 150;
		m_hotSpotShape.setFillColor(color);
		

		if (getActionMap()->isActive("Place"))
		{
			m_placingHotspot = false;
			sf::Color color = m_hotSpotShape.getFillColor();
			color.a = 255;
			m_hotSpotShape.setFillColor(color);
		}
	}
	// Changing player position
	else if (m_placingPlayer)
	{
		if (m_placingPlayerChartype == PLAYER_DEFENDER)
		{
			m_players[m_placingPlayerIndex]->defender->setPosition(current_mouse_pos);
		}
		else if (m_placingPlayerChartype == PLAYER_GATHERER)
		{
			m_players[m_placingPlayerIndex]->gatherer->setPosition(current_mouse_pos);
		}

		if (getActionMap()->isActive("Place"))
		{
			m_placingPlayer = false;
		}
	}
	else
	{
		// Click on an entity
		if (getActionMap()->isActive("Place"))
		{
			bool clicked = false;
			for (unsigned int i = 0; i < m_players.size(); i++)
			{
				if (Math::pointInCircle(current_mouse_pos, m_players[i]->defender->getPosition(), m_players[i]->defender->getRadius()))
				{
					m_placingPlayer = true;
					m_placingPlayerIndex = i;
					m_placingPlayerChartype = PLAYER_DEFENDER;
					clicked = true;
				}
				else if (Math::pointInCircle(current_mouse_pos, m_players[i]->gatherer->getPosition(), m_players[i]->gatherer->getRadius()))
				{
					m_placingPlayer = true;
					m_placingPlayerIndex = i;
					m_placingPlayerChartype = PLAYER_GATHERER;
					clicked = true;
				}
			}

			// Clicking on hotspot
			if (!clicked && Math::pointInCircle(current_mouse_pos, m_hotSpotShape.getPosition(), m_hotSpotShape.getRadius()))
			{
				clicked = true;
				m_placingHotspot = true;
			}
		}
	}

	if (getActionMap()->isActive("Set_Default"))
	{
		setDefault();
	}

	if (getActionMap()->isActive("Exit"))
	{
		return false;
	}
	return true;
}

void LevelEditorState::draw()
{
	m_stateAsset->windowManager->getWindow()->setView(*m_levelView);
	m_stateAsset->windowManager->getWindow()->draw(*m_backgroundSprite);
	m_stateAsset->windowManager->getWindow()->draw(m_hotSpotShape);
	
	for (auto &player: m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->gatherer);
		m_stateAsset->windowManager->getWindow()->draw(*player->defender);
	}

	for (auto &obj : objects)
	{
		m_stateAsset->windowManager->getWindow()->draw(*obj.sprite);
	}
}

void LevelEditorState::setupActions()
{
	m_actionMap->operator[]("Exit") = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
	m_actionMap->operator[]("MenuSelected") = thor::Action(sf::Event::MenuItemSelected);
	m_actionMap->operator[]("Drag") = thor::Action(sf::Keyboard::Space, thor::Action::Hold);
	m_actionMap->operator[]("Place") = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
	m_actionMap->operator[]("Center") = thor::Action(sf::Keyboard::LControl, thor::Action::Hold);
	m_actionMap->operator[]("Set_Default") = thor::Action(sf::Keyboard::LControl, thor::Action::Hold) && thor::Action(sf::Keyboard::D, thor::Action::PressOnce);
	m_actionMap->operator[]("Create_Object") = thor::Action(sf::Keyboard::LControl, thor::Action::Hold) && thor::Action(sf::Keyboard::B, thor::Action::PressOnce);
}

void LevelEditorState::openFile()
{
	OPENFILEINFO ofi;
	ofi.filters = "Level files\0*.level";
	ofi.caption = "Choose level file";
	std::string openpath = m_stateAsset->windowManager->browseFile(ofi);

	if (!openpath.empty())
	{
		std::ifstream istream;
		istream.open(openpath, std::ifstream::binary);
		if (!istream.is_open())
		{
			std::cout << "Failed to load level: Can't open file" << std::endl;
			return;
		}

		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(istream, root, false))
		{
			std::cout << "Failed to parse level " << openpath << " with message: " << reader.getFormatedErrorMessages() << std::endl;
			return;
		}

		m_backgroundPath = root["background"]["path"].asString();
		if (m_backgroundTexture->loadFromFile(m_backgroundPath))
		{
			m_backgroundSprite->setTexture(*m_backgroundTexture, true);
		}

		for (auto it = root["players"].begin(); it != root["players"].end(); ++it)
		{
			m_players[it.index()]->defender->setPosition(sf::Vector2f((*it)["defender_position_x"].asDouble(), (*it)["defender_position_y"].asDouble()));
			m_players[it.index()]->gatherer->setPosition(sf::Vector2f((*it)["gatherer_position_x"].asDouble(), (*it)["gatherer_position_y"].asDouble()));
		}

		objects.clear();
		for (auto it = root["objects"].begin(); it != root["objects"].end(); ++it)
		{
			loadObjectRaw((*it)["path"].asString());
		}
	}
}

void LevelEditorState::saveFile()
{
	OPENFILEINFO ofi;
	ofi.caption = "Save level";
	std::string savepath = m_stateAsset->windowManager->saveFile(ofi);
	if (!savepath.empty())
	{
		Json::Value rootNode;
		Json::Value backgroundNode;
		Json::Value playersNode;
		Json::Value objectsNode;

		for (int i = 0; i < 4; i++)
		{
			Json::Value playerNode;
			playerNode["defender_position_x"] = m_players[i]->defender->getPosition().x;
			playerNode["defender_position_y"] = m_players[i]->defender->getPosition().y;
			playerNode["gatherer_position_x"] = m_players[i]->gatherer->getPosition().x;
			playerNode["gatherer_position_y"] = m_players[i]->gatherer->getPosition().y;
			playersNode.append(playerNode);
		}

		std::cout << objects.size() << std::endl;
		for (int i = 0; i < objects.size(); i++)
		{
			Json::Value objectNode;
			//std::string path = FileSystem::getRelativePath(FileSystem::getCurrentDirectory().c_str(), objects[i].object_path.c_str());
			//objectNode["path"] = String::replace(path, "\\\\", "/");
			objectNode["path"] = objects[i].object_path;
			objectNode["x"] = static_cast<int>(objects[i].sprite->getPosition().x);
			objectNode["y"] = static_cast<int>(objects[i].sprite->getPosition().y);
			objectsNode.append(objectNode);
			std::cout << objectsNode.size() << std::endl;
		}

		backgroundNode["path"] = m_backgroundPath;

		rootNode["objects"] = objectsNode;
		rootNode["background"] = backgroundNode;
		rootNode["players"] = playersNode;

		Json::StyledWriter writer;
		std::string output = writer.write(rootNode);
		std::ofstream ofs;
		ofs.open(savepath);
		if (ofs.is_open())
		{
			ofs << output;
			ofs.close();
			std::string message = "Level was saved in " + savepath;
			MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), "Success", message.c_str(), 0);
		}
	}
}

void LevelEditorState::newFile()
{

}

void LevelEditorState::exitFile()
{
	m_exit = true;
}

void LevelEditorState::changeBackground()
{
	OPENFILEINFO ofi;
	ofi.caption = "Choose background image";
	std::string backgroundFile = m_stateAsset->windowManager->browseFile(ofi);
	if (!backgroundFile.empty())
	{
		if (m_backgroundTexture->loadFromFile(backgroundFile))
		{
			m_backgroundPath = FileSystem::getRelativePath(m_stateAsset->config->getRoot().c_str(), backgroundFile.c_str());
			m_backgroundSprite->setTexture(*m_backgroundTexture, true);
		}
		else
		{
			std::cout << "Image could not be loaded: " << backgroundFile << std::endl;
		}
	}
}

void LevelEditorState::handlePlayer(int player_id, int char_id)
{
	m_placingPlayer = true;
	m_placingPlayerChartype = char_id;
	m_placingPlayerIndex = player_id;
}

void LevelEditorState::setDefault()
{
	sf::Vector2u windowSize = m_stateAsset->windowManager->getWindow()->getSize();

	std::vector<sf::Color> colors;
	colors.push_back(sf::Color::Red);
	colors.push_back(sf::Color::Green);
	colors.push_back(sf::Color::Yellow);
	colors.push_back(sf::Color::Blue);

	std::vector<sf::Vector2f> defender_positions;
	defender_positions.push_back(sf::Vector2f(100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, 100));
	defender_positions.push_back(sf::Vector2f(windowSize.x - 100, windowSize.y - 100));
	defender_positions.push_back(sf::Vector2f(100, windowSize.y - 100));

	std::vector<sf::Vector2f> gatherer_positions;
	gatherer_positions.push_back(sf::Vector2f(50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, 50));
	gatherer_positions.push_back(sf::Vector2f(windowSize.x - 50, windowSize.y - 50));
	gatherer_positions.push_back(sf::Vector2f(50, windowSize.y - 50));

	for (int i = 0; i < 4; i++)
	{
		PlayerStruct* player = new PlayerStruct();
		player->defender = new sf::CircleShape();
		player->defender->setFillColor(colors[i]);
		player->defender->setPosition(defender_positions[i]);
		player->defender->setOrigin(DEFENDER_RADIUS, DEFENDER_RADIUS);
		player->defender->setRadius(DEFENDER_RADIUS);

		player->gatherer = new sf::CircleShape();
		player->gatherer->setFillColor(colors[i]);
		player->gatherer->setPosition(gatherer_positions[i]);
		player->gatherer->setOrigin(GATHERER_RADIUS, GATHERER_RADIUS);
		player->gatherer->setRadius(GATHERER_RADIUS);

		m_players.push_back(player);
	}

	m_backgroundPath = DEFAULT_BACKGROUND;
	m_backgroundSprite = new sf::Sprite();
	m_backgroundTexture = new sf::Texture();
	m_backgroundTexture->loadFromFile(DEFAULT_BACKGROUND);
	m_backgroundSprite->setTexture(*m_backgroundTexture);

	m_hotSpotShape.setRadius(100);
	m_hotSpotShape.setOrigin(m_hotSpotShape.getRadius(), m_hotSpotShape.getRadius());
	m_hotSpotShape.setPosition(m_backgroundSprite->getGlobalBounds().width / 2.f, m_backgroundSprite->getGlobalBounds().height / 2.f);
	m_hotSpotShape.setFillColor(sf::Color(100, 160, 100, 255));
}

void LevelEditorState::loadObject()
{
	OPENFILEINFO ofi;
	ofi.caption = "Choose background image";
	ofi.filters = "Object files\0*.objx";
	std::string objectFile = m_stateAsset->windowManager->browseFile(ofi);
	if (!objectFile.empty())
	{
		std::string path = FileSystem::getRelativePath(FileSystem::getCurrentDirectory().c_str(), objectFile.c_str());
		loadObjectRaw(String::replace(path, "\\\\", "/"));
	}
	else
	{
		MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), "Failed to load object file. A file was not selected.", "Failed to load object", 0);
	}
}

void LevelEditorState::handleMenuSelected(thor::ActionContext<std::string> context)
{
	m_stateAsset->windowManager->setPostFocus(true);
	sf::Event event = *context.event;
	switch (event.menuAction.identifier)
	{
	case ID_FILE_SAVE:
		saveFile();
		break;
	case ID_FILE_OPEN:
		openFile();
		break;
	case ID_FILE_NEW:
		newFile();
		break;
	case ID_FILE_EXIT:
		exitFile();
		break;
	case ID_BACKGROUND_CHANGE:
		changeBackground();
		break;

	case ID_PLAYER_1_DEFENDER: handlePlayer(PLAYER_1, PLAYER_DEFENDER); break;
	case ID_PLAYER_1_GATHERER: handlePlayer(PLAYER_1, PLAYER_GATHERER); break;
	case ID_PLAYER_2_DEFENDER: handlePlayer(PLAYER_2, PLAYER_DEFENDER); break;
	case ID_PLAYER_2_GATHERER: handlePlayer(PLAYER_2, PLAYER_GATHERER); break;
	case ID_PLAYER_3_DEFENDER: handlePlayer(PLAYER_3, PLAYER_DEFENDER); break;
	case ID_PLAYER_3_GATHERER: handlePlayer(PLAYER_3, PLAYER_GATHERER); break;
	case ID_PLAYER_4_DEFENDER: handlePlayer(PLAYER_4, PLAYER_DEFENDER); break;
	case ID_PLAYER_4_GATHERER: handlePlayer(PLAYER_4, PLAYER_GATHERER); break;

	case ID_DEFAULT:
		setDefault();
		break;
	case ID_LOAD_OBJECT:
		loadObject();
		break;
	default:
		break;
	}
}

void LevelEditorState::loadObjectRaw(std::string objectFile)
{
	std::ifstream istream;
	istream.open(objectFile, std::ifstream::binary);
	if (!istream.is_open())
	{
		std::string content = "Failed to load object file: " + objectFile;
		MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), content.c_str(), "Failed to load object", 0);
		return;
	}

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(istream, root, false))
	{
		std::string content = "Failed to parse object file: " + objectFile;
		MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), content.c_str(), "Failed to parse object", 0);
		return;
	}

	bool parseError = false;

	// object data
	bool use_circle = false;
	sf::Vector2f circle_position(0, 0);
	float circle_radius = 0;
	std::string image_path;
	std::vector<sf::Vector2f> points;

	if (!root["use_circle"].isNull() && root["use_circle"].isBool())
	{
		if (root["use_circle"].asBool())
		{
			use_circle = true;

			if (!root["circle"].isNull() && root["circle"].isObject())
			{
				if (!root["circle"]["position"].isNull() && root["circle"].isObject())
				{
					if (!root["circle"]["position"]["x"].isNull() && root["circle"]["position"]["x"].isInt())
					{
						circle_position.x = root["circle"]["position"]["x"].asInt();
					}

					if (!root["circle"]["position"]["y"].isNull() && root["circle"]["position"]["y"].isInt())
					{
						circle_position.y = root["circle"]["position"]["y"].asInt();
					}
				}

				if (!root["circle"]["radius"].isNull() && root["circle"]["radius"].isInt())
				{
					circle_radius = root["circle"]["radius"].asInt();
				}
			}
		}
		else
		{
			if (!root["points"].isNull() && root["points"].isArray())
			{
				if (root["points"].size() > 1)
				{
					for (auto it = root["points"].begin(); it != root["points"].end(); ++it)
					{
						sf::Vector2f point;
						if (!(*it)["x"].isNull() && (*it)["x"].isInt())
						{
							point.x = (*it)["x"].asInt();
						}
						if (!(*it)["y"].isNull() && (*it)["y"].isInt())
						{
							point.y = (*it)["y"].asInt();
						}
						points.push_back(point);
					}
				}
			}
			else
			{
				parseError = true;
			}
		}

		if (!root["image"].isNull() && root["image"].isObject())
		{
			if (!root["image"]["path"].isNull() && root["image"]["path"].isString())
			{
				image_path = root["image"]["path"].asString();
			}
			else
			{
				parseError = true;
			}
		}
		else
		{
			parseError = true;
		}
	}
	else
	{
		parseError = true;
	}

	if (parseError)
	{
		MessageBoxA(m_stateAsset->windowManager->getWindow()->getSystemHandle(), "Failed to parse object. Incorrect formatting.", "Failed to parse object", 0);
	}
	else
	{
		Object obj;
		obj.circle_position = circle_position;
		obj.circle_radius = circle_radius;
		obj.image_path = image_path;
		obj.object_path = objectFile;
		obj.use_circle_collision = use_circle;
		obj.points = points;
		obj.sprite = new sf::Sprite();
		obj.sprite->setTexture(m_stateAsset->resourceHolder->getTexture(obj.image_path, false));
		obj.sprite->setPosition(sf::Vector2f(thor::random(100, 600), thor::random(100, 600)));
		objects.push_back(obj);
	}
}

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lparam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		ShowWindow(hwnd, SW_SHOW);
		break;
	}
	return false;
}