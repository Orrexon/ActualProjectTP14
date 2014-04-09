#include <fstream>
#include <json\json.h>

#include "Level.h"
#include "LevelLoader.h"

LevelLoader::LevelLoader()
{
}


LevelLoader::~LevelLoader()
{
}

Level* LevelLoader::parseLevel(const std::string &filepath)
{
	std::ifstream istream;
	istream.open(m_levelDirectory + filepath, std::ifstream::binary);
	if (!istream.is_open())
	{
		std::cout << "Failed to load level: Can't open file" << std::endl;
		return nullptr;
	}

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(istream, root, false))
	{
		std::cout << "Failed to parse level " << m_levelDirectory + filepath << " with message: " << reader.getFormatedErrorMessages() << std::endl;
		return nullptr;
	}
	Level* level = new Level();

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
		delete level;
		level = nullptr;
		return nullptr;
	}

	level->setBackgroundPath(root["background"]["path"].asString());
	for (auto it = root["players"].begin(); it != root["players"].end(); ++it)
	{
		std::cout << it.index() << std::endl;
		std::cout << (*it)["defender_position_x"].asDouble() << std::endl;
		std::cout << (*it)["defender_position_y"].asDouble() << std::endl;
		std::cout << (*it)["gatherer_position_x"].asDouble() << std::endl;
		std::cout << (*it)["gatherer_position_y"].asDouble() << std::endl;
		level->setDefenderSpawn(it.index(), (*it)["defender_position_x"].asDouble(), (*it)["defender_position_y"].asDouble());
		level->setGathererSpawn(it.index(), (*it)["gatherer_position_x"].asDouble(), (*it)["gatherer_position_y"].asDouble());
	}
	return level;
}

void LevelLoader::setDirectory(const std::string &filepath)
{
	m_levelDirectory = filepath;
}