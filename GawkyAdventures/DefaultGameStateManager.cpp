#include "DefaultGameStateManager.h"
#include "GameState.h"

#include <stdexcept>

DefaultGameStateManager::DefaultGameStateManager(){}

void DefaultGameStateManager::Init(){}

DefaultGameStateManager::~DefaultGameStateManager()
	{
		while (!this->activeStates.empty())
		{
			Pop();
		}
	}

	std::shared_ptr<GameState> DefaultGameStateManager::Peek() const
	{
		if (this->activeStates.empty())
		{
			return std::shared_ptr<GameState>();
		}
		else
		{
			return this->activeStates.at(this->activeStates.size() - 1).first;
		}
	}

	void DefaultGameStateManager::Push(const std::shared_ptr<GameState> &state, MODE mode)
	{
		this->activeStates.push_back(std::make_pair(state, mode));

		notifyObscuredStates();

		state->Entered();
	}

	std::shared_ptr<GameState> DefaultGameStateManager::Pop()
	{
		if (this->activeStates.empty())
		{
			throw std::runtime_error("Attemped to pop from an empty game state stack");
		}

		GameStateModailtyPair popped = this->activeStates.at(this->activeStates.size() - 1);
		popped.first->Exiting();
		this->activeStates.pop_back();

		notifyRevealedStates();

		return popped.first;
	}

	void DefaultGameStateManager::Update(float dt)
	{
		this->activeStates.at(this->activeStates.size() - 1).first->Update(dt);
	}

	void DefaultGameStateManager::Draw()
	{
		this->activeStates.at(this->activeStates.size() - 1).first->Draw();
	}

	void DefaultGameStateManager::notifyObscuredStates()
	{
		if (this->activeStates.size() < 2)
		{
			return;
		}

		// Reverse scan until we hit either the beginning or find the next Hiding state
		std::size_t i = this->activeStates.size() - 2;
		while (i > 0)
		{
			if (this->activeStates.at(i).second == EXCLUSIVE)
			{
				break;
			}
			--i;
		}

		// now go forward 9up until the second last state) and notify the obscured states
		while (i < this->activeStates.size() - 1)
		{
			this->activeStates.at(i).first->Obscuring();
			++i;
		}
	}

	void DefaultGameStateManager::notifyRevealedStates()
	{
		if (this->activeStates.empty())
		{
			return;
		}

		// Reverse scan until we hit the beginning or find the next Hiding state
		std::size_t i = this->activeStates.size() - 1;
		while (i > 0)
		{
			if (this->activeStates.at(i).second == EXCLUSIVE)
			{
				break;
			}
			--i;
		}

		// Now go forward and notify all revealed state
		while (i < this->activeStates.size())
		{
			this->activeStates.at(i).first->Revealed();
			++i;
		}
	}
