#pragma once
#include "Singleton.h"

namespace diji
{
    enum class GameState
    {
        Invalid = -1,
    };
    
    class GameStateManager final : public Singleton<GameStateManager>
    {
    public:
        GameState GetCurrentGameState() const { return m_CurrentGameState; }
        void SetNewGameState(const GameState& newGameState) { m_CurrentGameState = newGameState; }

    private:
        GameState m_CurrentGameState = GameState::Invalid;
    };
}