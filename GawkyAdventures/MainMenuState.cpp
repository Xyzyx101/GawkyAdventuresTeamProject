#include <memory>
#include <iostream>

#include "MainMenuState.h"

using namespace std;

void MainMenuState::Entered()
{
	cout << "MainMenu game state entered" << endl;
}

void MainMenuState::Exiting()
{
	cout << "MainMenu state is exiting" << endl;
}

void MainMenuState::Update(float elapsdtedTime)
{
	cout << "MainMenu state has been updated" << endl;
}

void MainMenuState::Draw()
{
	cout << "MainMenu state has been drawn" << endl;
}
