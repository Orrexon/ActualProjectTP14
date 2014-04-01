#pragma once

#include "GameState.h"

class PlayState : public GameState
{
public:
	PlayState();
	~PlayState();

	virtual void entering();
	virtual void leaving();
	virtual void obscuring();
	virtual void releaving();
	virtual bool update(float dt);
	virtual void draw();
};

