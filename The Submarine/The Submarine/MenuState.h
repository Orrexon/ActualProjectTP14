#pragma once

#include "GameState.h"

class MenuState : public GameState
{
public:
	MenuState();
	~MenuState();

	virtual void entering();
	virtual void leaving();
	virtual void obscuring();
	virtual void releaving();
	virtual bool update(float dt);
	virtual void draw();
};

