#ifndef GAMESTATE_H
#define GAMESTATE_H

	class GameState
	{
	public:
		// Destroy the game state
		virtual ~GameState() {};
		// Notifies the game state it is about to be exited
		virtual void Exiting() = 0;
		// Notifies the game state that it has been entered
		virtual void Entered() = 0;
		// Notifies the games state that it has been obscured by another state
		virtual void Obscuring() = 0;
		// Notifies the game state that it is no longer obscured by another state
		virtual void Revealed() = 0;

		virtual void Update( float dt ) = 0;
		virtual void Draw() = 0;
	};
#endif //GAMESTATE_H
