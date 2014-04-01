#pragma once

#include <string>

namespace sf
{
	class RenderWindow;
}

class WindowManager
{
public:
	WindowManager(std::string p_title);
	~WindowManager();

	void setWindowIcon(std::string p_filepath);
	sf::RenderWindow* getWindow();

	void setFocus(bool p_value);
	bool isInFocus();

private:
	sf::RenderWindow* m_window;
	bool m_inFocus;
};

