#ifndef DEFAULTGAMESTATEMANAGER_H
#define DEFAULTGAMESTATEMANAGER_H

#include <vector>
#include <utility>
#include "d3dApp.h"
#include "TextureMgr.h"

#include "GameStateManager.h"

	//Stacked game state manager
	class DefaultGameStateManager : public GameStateManager
	{
	private:
		// Stores a game state and the modality it was activated with
		typedef std::pair<std::shared_ptr<GameState>, MODE> GameStateModailtyPair;
		// Rebuilds the separate updateable and Drawbale queues when a hiding state has been popped back from the stack
		void notifyObscuredStates();
		// Notifies the previously exposed states that they have been revealed
		void notifyRevealedStates();
		// Stores the currently active game states
		std::vector<GameStateModailtyPair> activeStates;
	public:
		//Initializes a new game state manager
		DefaultGameStateManager();
		//Initialize the gameState
		void Init();
		//Destroy the game state manager, leaving and dropping any active game state
		virtual ~DefaultGameStateManager();
		// returns the lastmost game state on the stack
		virtual std::shared_ptr<GameState> Peek() const;
		// Appends a new game state to the stack
		virtual void Push(const std::shared_ptr<GameState> &state, MODE mode = EXCLUSIVE);
		// Removes the last most game state form the stack, returns the state removed from the stack
		virtual std::shared_ptr<GameState> Pop();
		// advances the time of the active game states
		void Update(float dt);
		// Instructs the active game states to render themselves or to update the scene graph
		void Draw();
	};

#endif //DEFAULTGAMESTATEMANAGER_H