#pragma once

#include <string>
#include <Windows.h>

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

	void setMenu(HMENU menu);
	std::string browseFile(std::string title = "");
	std::string saveFile(std::string title = "");
	void setFocus(bool p_value);
	bool isInFocus();

private:
	sf::RenderWindow* m_window;
	bool m_inFocus;
};

