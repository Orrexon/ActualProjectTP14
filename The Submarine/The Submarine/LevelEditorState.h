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
	void handleMenuSelected(thor::ActionContext<std::string> context);

private:
	Json::Value* m_jsonRoot;
	thor::ActionMap<std::string>::CallbackSystem* m_system;
};

