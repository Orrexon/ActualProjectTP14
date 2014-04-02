#include <Thor\Input.hpp>
#include <SFML\Graphics.hpp>
#include <iostream>
#include <Windows.h>
#include <json\json.h>

#include "LevelEditorState.h"
#include "GameState.h"
#include "GameStateManager.h"
#include "WindowManager.h"

enum
{
	ID_FILE_NEW,
	ID_FILE_OPEN,
	ID_FILE_SAVE,
	ID_FILE_EXIT,
	ID_BACKGROUND_CHANGE,
};

LevelEditorState::LevelEditorState()
{

}

LevelEditorState::~LevelEditorState()
{

}

void LevelEditorState::entering()
{
	m_jsonRoot = new Json::Value();
	m_exclusive = false;
	std::cout << "Entering level editor state" << std::endl;

	HMENU menuBar = CreateMenu();
	HMENU hFileDropDown = CreateMenu();
	HMENU hBackgroundDropDown = CreateMenu();
	HMENU hPlayerDropDown = CreateMenu();
	HMENU hHotSpotDropDown = CreateMenu();
	HMENU hPowerupsDropDown = CreateMenu();

	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hFileDropDown, "File");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hBackgroundDropDown, "Background");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hPlayerDropDown, "Player");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hHotSpotDropDown, "HotSpot");
	AppendMenuA(menuBar, MF_POPUP, (UINT_PTR)hPowerupsDropDown, "Powerups");
	
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_NEW, "New");
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_OPEN, "Open");
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_SAVE, "Save");
	AppendMenuA(hFileDropDown, MF_STRING, ID_FILE_EXIT, "Exit (Menu state)");
	
	m_stateAsset->windowManager->setMenu(menuBar);

}

void LevelEditorState::leaving()
{
	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_jsonRoot;
	m_jsonRoot = nullptr;
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
	MSG msg;
	while (PeekMessageA(&msg, m_stateAsset->windowManager->getWindow()->getSystemHandle(), 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_COMMAND)
		{
			switch (LOWORD(msg.wParam))
			{
			case ID_FILE_OPEN:
				openFile();
				break;
			case ID_FILE_SAVE:
				saveFile();
				break;
			case ID_FILE_NEW:
				newFile();
				break;
			}
		}

		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		return false;
	}
	return true;
}

void LevelEditorState::draw()
{

}

void LevelEditorState::setupActions()
{
	m_actionMap->operator[]("Exit") = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
}

void LevelEditorState::openFile()
{
	m_stateAsset->windowManager->browseFile("");
}

void LevelEditorState::saveFile()
{
	Json::StyledWriter writer;
	writer.write(m_jsonRoot);
	m_stateAsset->windowManager->saveFile();
}

void LevelEditorState::newFile()
{

}