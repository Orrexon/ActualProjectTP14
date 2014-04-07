#include <SFML\Graphics\Image.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#include <iostream>
#include <algorithm>
#include <thread>
#include "System.hpp"
#include "WindowManager.h"

WindowManager::WindowManager(std::string p_title)
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;
	m_postFocus = false;
	m_window = new sf::RenderWindow(sf::VideoMode(800, 600), p_title, sf::Style::None, settings);
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

std::string WindowManager::browseFile(OPENFILEINFO &ofi)
{
	auto filename = CreateZeroed<CHAR, 65536>();
	auto open_filename = CreateZeroed<OPENFILENAMEA>();

	open_filename.lStructSize = sizeof(OPENFILENAMEA);
	open_filename.lpstrFilter = "All files(*.*)\0*.*\0";
	open_filename.lpstrFile = filename.data();
	open_filename.nMaxFile = filename.size();
	open_filename.lpstrTitle = ofi.caption.c_str();
	open_filename.hwndOwner = m_window->getSystemHandle();
	open_filename.Flags = ofi.flags;

	auto result = GetOpenFileNameA(&open_filename);

	if (!result)
	{
		return "";
	}
	return std::string(std::begin(filename), std::end(filename));
}

std::string WindowManager::saveFile(std::string title)
{
	auto filename = CreateZeroed<TCHAR, 65536>();
	auto open_filename = CreateZeroed<OPENFILENAMEW>();

	open_filename.lStructSize = sizeof(OPENFILENAMEW);
	open_filename.lpstrFilter = L"Din mamma filer\0*.*\0";
	open_filename.lpstrFile = filename.data();
	open_filename.nMaxFile = filename.size();
	open_filename.lpstrTitle = L"Your mom";
	open_filename.hwndOwner = m_window->getSystemHandle();
	open_filename.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	auto result = GetSaveFileNameW(&open_filename);
	
	if (!result)
	{
		return "";
	}
	return std::string(std::begin(filename), std::end(filename));
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

bool WindowManager::postFocus()
{
	return m_postFocus;
}

void WindowManager::setPostFocus(bool p_value)
{
	m_postFocus = p_value;
}