#pragma once

#include <string>

class Level;

class LevelLoader
{
public:
	LevelLoader();
	~LevelLoader();

	Level* parseLevel(const std::string &filepath);
	void setDirectory(const std::string &filepath);

private:
	std::string m_levelDirectory;
};

