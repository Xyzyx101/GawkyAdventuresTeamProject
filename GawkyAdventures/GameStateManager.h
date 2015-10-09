#ifndef GAMESTATEMANAGER_H
#define GAMESTATEMANAGER_H

#include <memory>
	
	enum MODE {
		EXCLUSIVE,
		POPUP
	};

	//forward declaration so we don't need a header
	class GameState;
//--------------------------
	//Interface for stack-based game state manager
	class GameStateManager
	{
	public:
		// Destroy the game state manager, leave and drop any active game state
		virtual ~GameStateManager(){}
		// Return the current active game state
		virtual std::shared_ptr<GameState> Peek() const = 0;
		// appends a new game state to the stack
		virtual void Push( const std::shared_ptr<GameState> &state, MODE mode = EXCLUSIVE ) = 0;
		// Removes the lastmost game state form the stack
		virtual std::shared_ptr<GameState> Pop() = 0;
		// Replaces the lastmost game state on the stack
		virtual std::shared_ptr<GameState> Switch(const std::shared_ptr<GameState> &state, MODE mode = EXCLUSIVE)
		{
			std::shared_ptr<GameState> currentState = Peek();
			if (currentState)
			{
				Pop();
			}

			Push(state, mode);

			return currentState;
		}
	};
#endif // GAMESTATEMANAGER_H
