#include <SFML\Graphics\Image.hpp>
#include <SFML\Graphics\RenderWindow.hpp>

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

void WindowManager::setFocus(bool p_value)
{
	m_inFocus = p_value;
}

bool WindowManager::isInFocus()
{
	return m_inFocus;
}