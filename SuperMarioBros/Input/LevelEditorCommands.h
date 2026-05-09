#pragma once
#include "Engine/Input/GameActorCommand.h"

namespace superMarioBros
{
    class SelectorControls;
}

namespace superMarioBros
{
    class PlayerInputManager;

    class MoveSelector final : public diji::GameActorCommands
    {
    public:
        explicit MoveSelector(diji::GameObject* actor, const sf::Vector2f& direction, bool isStart);
        ~MoveSelector() noexcept override = default;

        void Execute() override;

    private:
        SelectorControls* m_Selector = nullptr;
        const sf::Vector2f m_Direction;
        bool m_IsStart;
    };

    class SelectMenuItem final : public diji::GameActorCommands
    {
    public:
        explicit SelectMenuItem(diji::GameObject* actor);
        ~SelectMenuItem() noexcept override = default;

        void Execute() override;
        
    private:
        SelectorControls* m_Selector = nullptr;
    };

    class ClearSpecialMenu final : public diji::GameActorCommands
    {
    public:
        explicit ClearSpecialMenu(diji::GameObject* actor);
        ~ClearSpecialMenu() noexcept override = default;

        void Execute() override;
        
    private:
        SelectorControls* m_Selector = nullptr;
    };
    
}
