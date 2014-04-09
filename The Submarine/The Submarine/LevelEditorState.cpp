#pragma comment(lib, "Shlwapi.lib")

#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <json\json.h>
#include <fstream>
#include <Shlwapi.h>
#include <fstream>


#include "LevelEditorState.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "WindowManager.h"
#include "resource.h"
#include "Math.h"
#include "String.h"

enum
{
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
	ID_FILE_EXIT,
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
	ID_HOTSPOT_RADIUS
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

enum
{
	DEFENDER_RADIUS = 30,
	GATHERER_RADIUS = 15
};

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

	for (int i = 0; i < 4; i++)
	{
		PlayerStruct* player = new PlayerStruct();
		player->defender = new sf::CircleShape();
		player->defender->setFillColor(colors[i]);
		player->defender->setPosition(-100, -100);
		player->defender->setOrigin(DEFENDER_RADIUS, DEFENDER_RADIUS);
		player->defender->setRadius(DEFENDER_RADIUS);

		player->gatherer = new sf::CircleShape();
		player->gatherer->setFillColor(colors[i]);
		player->gatherer->setPosition(-100, -100);
		player->gatherer->setOrigin(GATHERER_RADIUS, GATHERER_RADIUS);
		player->gatherer->setRadius(GATHERER_RADIUS);

		m_players.push_back(player);
	}

	m_backgroundPath = "";
	m_backgroundSprite = new sf::Sprite();
	m_backgroundTexture = new sf::Texture();
	m_backgroundTexture->loadFromFile("../resources/backgrounds/bg.png");
	m_backgroundSprite->setTexture(*m_backgroundTexture);

	m_hotspotPath = "";
	m_hotSpotSprite = new sf::Sprite();
	m_hotSpotTexture = new sf::Texture();
	m_hotSpotTexture->loadFromFile("../resources/hotspot.psd");
	m_hotSpotSprite->setTexture(*m_hotSpotTexture);
	m_hotSpotSprite->setOrigin(m_hotSpotTexture->getSize().x / 2, m_hotSpotTexture->getSize().y / 2);
	m_hotSpotSprite->setPosition(windowSize.x / 2, windowSize.y / 2);

	m_levelView = new sf::View();
	m_levelView->setSize(sf::Vector2f(windowSize.x, windowSize.y));
	m_levelView->setCenter(windowSize.x / 2, windowSize.y / 2);
	
	m_jsonRoot = new Json::Value();
	m_exclusive = false;

	std::cout << "Entering level editor state" << std::endl;

	HMENU menuBar = CreateMenu();
	HMENU hFileDropDown = CreateMenu();
	HMENU hBackgroundDropDown = CreateMenu();
	HMENU hPlayerDropDown = CreatePopupMenu();
	HMENU hHotSpotDropDown = CreateMenu();
	HMENU hPowerupsDropDown = CreateMenu();
	HMENU hPlayer1Sub = CreatePopupMenu();
	HMENU hPlayer2Sub = CreatePopupMenu();
	HMENU hPlayer3Sub = CreatePopupMenu();
	HMENU hPlayer4Sub = CreatePopupMenu();


	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hFileDropDown, "File");
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
	AppendMenuA(hFileDropDown, MF_SEPARATOR | MF_BYPOSITION, 0, nullptr);
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_EXIT, "Exit");

	AppendMenuA(hBackgroundDropDown, MF_STRING, ID_BACKGROUND_CHANGE, "Change background");

	HBITMAP p1_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP1));
	HBITMAP p2_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP2));
	HBITMAP p3_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP3));
	HBITMAP p4_icon = LoadBitmapW(GetModuleHandleW(nullptr), MAKEINTRESOURCE(IDB_BITMAP4));
	
	DWORD dLastError = GetLastError();
	LPCTSTR strErrorMessage = NULL;

	FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
		FORMAT_MESSAGE_ARGUMENT_ARRAY | FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL,
		dLastError,
		0,
		(LPWSTR)&strErrorMessage,
		0,
		NULL);

	//Prints debug output to the console
	std::wcout << strErrorMessage << std::endl;


	SetMenuItemBitmaps(hPlayerDropDown, 0, MF_BYPOSITION, p1_icon, p1_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 1, MF_BYPOSITION, p2_icon, p2_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 2, MF_BYPOSITION, p3_icon, p3_icon);
	SetMenuItemBitmaps(hPlayerDropDown, 3, MF_BYPOSITION, p4_icon, p4_icon);

	RECT rect;
	GetClientRect(m_stateAsset->windowManager->getWindow()->getSystemHandle(), &rect);
	std::cout << rect.bottom << std::endl;
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
	delete m_hotSpotSprite;
	m_backgroundSprite = nullptr;
	m_hotSpotSprite = nullptr;

	delete m_backgroundTexture;
	delete m_hotSpotTexture;
	m_backgroundTexture = nullptr;
	m_hotSpotTexture = nullptr;

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
	if (m_dialogEditHotspot != nullptr)
	{
		MSG msg;
		while (GetMessage(&msg, m_dialogEditHotspot, 0, 0) > 0) {
			if (!IsDialogMessage(m_dialogEditHotspot, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

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
		m_hotSpotSprite->setColor(sf::Color(255, 255, 255, 150));
		m_hotSpotSprite->setPosition(current_mouse_pos);
		if (getActionMap()->isActive("Place"))
		{
			m_placingHotspot = false;
			m_hotSpotSprite->setColor(sf::Color(255, 255, 255, 255));
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
			if (!clicked && Math::pointInCircle(current_mouse_pos, m_hotSpotSprite->getPosition(), m_hotSpotSprite->getGlobalBounds().width / 2))
			{
				clicked = true;
				m_placingHotspot = true;
			}
		}
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
	m_stateAsset->windowManager->getWindow()->draw(*m_hotSpotSprite);
	
	for (auto &player: m_players)
	{
		m_stateAsset->windowManager->getWindow()->draw(*player->gatherer);
		m_stateAsset->windowManager->getWindow()->draw(*player->defender);
	}
}

void LevelEditorState::setupActions()
{
	m_actionMap->operator[]("Exit") = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
	m_actionMap->operator[]("MenuSelected") = thor::Action(sf::Event::MenuItemSelected);
	m_actionMap->operator[]("Drag") = thor::Action(sf::Keyboard::Space, thor::Action::Hold);
	m_actionMap->operator[]("Place") = thor::Action(sf::Mouse::Left, thor::Action::PressOnce);
}

void LevelEditorState::openFile()
{
	OPENFILEINFO ofi;
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

		bool parseError = false;
		std::cout << "Parsing level..." << std::endl;
		if (root.isObject())
		{
			if (root["background"].isNull() || !root["background"].isObject())
			{
				parseError = true;
				std::cout << "ERROR: Background parsing" << std::endl;
			}
			else
			{
				if (root["background"]["path"].isNull() || !root["background"]["path"].isString())
				{
					parseError = true;
					std::cout << "ERROR: Background path parsing" << std::endl;
				}
			}
			if (root["players"].isNull() || !root["players"].isArray())
			{
				parseError = true;
				std::cout << "ERROR: Players parsing" << std::endl;
			}
			else
			{
				if (root["players"].size() != 4)
				{
					parseError = true;
					std::cout << "ERROR: Player count is not equal to four" << std::endl;
				}
				for (auto it = root["players"].begin(); it != root["players"].end(); ++it)
				{
					if ((*it)["defender_position_x"].isNull() || !(*it)["defender_position_x"].isInt()) { parseError = true; }
					if ((*it)["defender_position_y"].isNull() || !(*it)["defender_position_y"].isInt()) { parseError = true; }
					if ((*it)["gatherer_position_x"].isNull() || !(*it)["gatherer_position_x"].isInt()) { parseError = true; }
					if ((*it)["gatherer_position_y"].isNull() || !(*it)["gatherer_position_y"].isInt()) { parseError = true; }
				}
			}
		}
		if (!parseError)
		{
			std::cout << "Errors were found during parsing level" << std::endl;
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
		for (int i = 0; i < 4; i++)
		{
			Json::Value playerNode;
			playerNode["defender_position_x"] = m_players[i]->defender->getPosition().x;
			playerNode["defender_position_y"] = m_players[i]->defender->getPosition().y;
			playerNode["gatherer_position_x"] = m_players[i]->gatherer->getPosition().x;
			playerNode["gatherer_position_y"] = m_players[i]->gatherer->getPosition().y;
			playersNode.append(playerNode);
		}


		Json::Value backgroundPath(m_backgroundPath);

		char s[255];
		DWORD a = GetModuleFileNameA(GetModuleHandle(nullptr), s, 255);
		char *lpStr1;
		lpStr1 = s;
		PathRemoveFileSpecA(lpStr1);

		std::string backgroundRelative = String::GetRelativePath(s, m_backgroundPath.c_str());

		backgroundNode["path"] = backgroundRelative;

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
			m_backgroundPath = backgroundFile;
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

void LevelEditorState::chooseHotspotImage()
{
	OPENFILEINFO ofi;
	ofi.caption = "Choose hotspot image";
	std::string file = m_stateAsset->windowManager->browseFile(ofi);
	if (!file.empty())
	{
		if (m_hotSpotTexture->loadFromFile(file))
		{
			m_hotspotPath = file;
			m_hotSpotSprite->setTexture(*m_hotSpotTexture, true);
			m_hotSpotSprite->setOrigin(m_hotSpotTexture->getSize().x / 2, m_hotSpotTexture->getSize().y / 2);
		}
		else
		{
			std::cout << "Image could not be loaded: " << file << std::endl;
		}
	}
}

void LevelEditorState::setHotspotRadius()
{
	HWND dialog = (HWND)DialogBoxA(GetModuleHandle(nullptr), MAKEINTRESOURCEA(CHR_DIALOG), m_stateAsset->windowManager->getWindow()->getSystemHandle(), reinterpret_cast<DLGPROC>(DialogProc), 0L);
	MSG message;
	while (GetMessage(&message, NULL, 0, 0) > 0)
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
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

	case ID_HOTSPOT_IMAGE:
		chooseHotspotImage();
		break;

	case ID_HOTSPOT_RADIUS:
		std::cout << "CHANGE HOTSPOT RADIUS" << std::endl;
		setHotspotRadius();
		break;
	}
}

BOOL CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lparam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		ShowWindow(hwnd, SW_SHOW);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case CHR_INPUT:
			std::cout << "I am now typing inside the input box" << std::endl;
			break;
		case CHR_SAVE:
			std::cout << "I am clicking on the save button" << std::endl;
			break;
		}
		break;
	}
	return false;
}