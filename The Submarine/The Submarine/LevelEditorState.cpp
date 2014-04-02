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

	m_system = new thor::ActionMap<std::string>::CallbackSystem;
	m_system->connect("MenuSelected", std::bind(&LevelEditorState::handleMenuSelected, this, std::placeholders::_1));
}

void LevelEditorState::leaving()
{
	delete m_actionMap;
	m_actionMap = nullptr;

	delete m_jsonRoot;
	m_jsonRoot = nullptr;

	delete m_system;
	m_system = nullptr;
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
	getActionMap()->invokeCallbacks(*m_system, nullptr);
	return true;
}

void LevelEditorState::draw()
{

}

void LevelEditorState::setupActions()
{
	m_actionMap->operator[]("Exit") = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
	m_actionMap->operator[]("MenuSelected") = thor::Action(sf::Event::MenuItemSelected);
}

void LevelEditorState::openFile()
{
	m_stateAsset->windowManager->browseFile("");
}

void LevelEditorState::saveFile()
{
	std::cout << "Saving file..." << std::endl;
	Json::StyledWriter writer;
	writer.write(m_jsonRoot);
	m_stateAsset->windowManager->saveFile("");
	std::cout << "Saving done..." << std::endl;
}

void LevelEditorState::newFile()
{

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
	}
}