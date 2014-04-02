#include <SFML\Graphics\Image.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <Windows.h>

#include "WindowManager.h"

WindowManager::WindowManager(std::string p_title)
{
	m_window = new sf::RenderWindow(sf::VideoMode(800, 600), p_title);
	m_window->setVerticalSyncEnabled(true);
	m_window->setKeyRepeatEnabled(false);
}


WindowManager::~WindowManager()
{
}

void WindowManager::setWindowIcon(std::string p_filepath)
{
	sf::Image image;
	if (image.loadFromFile(p_filepath))
	{
		m_window->setIcon(image.getSize().x, image.getSize().y, image.getPixelsPtr());
	}
}

sf::RenderWindow* WindowManager::getWindow()
{
	return m_window;
}

std::string WindowManager::browseFile(std::string title)
{
	char buf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);

	OPENFILENAMEA ofn;
	char szFile[255];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof (ofn);
	ofn.hwndOwner = m_window->getSystemHandle();
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = ".png (SWAG)\0*.png*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = title.c_str();
	ofn.lpstrInitialDir = buf;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	GetOpenFileNameA(&ofn);
	return szFile;
}

std::string WindowManager::saveFile(std::string title)
{
	char buf[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buf);

	OPENFILENAMEA ofn;
	char szFile[255];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof (ofn);
	ofn.hwndOwner = m_window->getSystemHandle();
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = title.c_str();
	ofn.lpstrInitialDir = buf;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	GetSaveFileNameA(&ofn);
	return szFile;
}

void WindowManager::setMenu(HMENU menu)
{
	SetMenu(m_window->getSystemHandle(), menu);
}

void WindowManager::setFocus(bool p_value)
{
	m_inFocus = p_value;
}

bool WindowManager::isInFocus()
{
	return m_inFocus;
}