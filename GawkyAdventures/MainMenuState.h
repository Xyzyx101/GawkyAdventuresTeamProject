#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include <memory>
#include <iostream>

#include "DefaultGameStateManager.h"
#include "GameState.h"


class MainMenuState : public GameState
{
public:
	void Entered();
	void Exiting();
	void Update(float dt);
	void Draw();

private:
	std::shared_ptr<GameStateManager> gameStateManager;
};

#endif //MAINMENUSTATE_H
