#pragma once

#include "GameState.h"

namespace Json
{
	class Value;
}

class LevelEditorState : public GameState
{
public:
	LevelEditorState();
	~LevelEditorState();

	void entering();
	void leaving();
	void obscuring();
	void releaving();
	bool update(float dt);
	void draw();
	void setupActions();

	void openFile();
	void saveFile();
	void newFile();

private:
	Json::Value* m_jsonRoot;
};

