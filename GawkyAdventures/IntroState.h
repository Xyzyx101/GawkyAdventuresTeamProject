#ifndef INTROSTATE_H
#define INTROSTATE_H

#include <memory>
#include <iostream>

#include "DefaultGameStateManager.h"
#include "GameState.h"


class IntroState : public GameState
{
public:
	void Entered();
	void Exiting();
	void Update(float dt);
	void Draw();

private:
	std::shared_ptr<GameStateManager> gameStateManager;
};
#endif //INTROSTATE_H
