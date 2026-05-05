#pragma once
#include "Engine/Input/GameActorCommand.h"

namespace superMarioBros
{
    class Selector;
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
        Selector* m_Selector = nullptr;
        const sf::Vector2f m_Direction;
        bool m_IsStart;
    };
}
