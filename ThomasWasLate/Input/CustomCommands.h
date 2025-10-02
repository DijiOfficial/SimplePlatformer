#pragma once
#include "Engine/Input/GameActorCommand.h"

namespace thomasWasLate
{
    class PlayerCharacter;

    class MoveCharacter final : public diji::GameActorCommands
    {
    public:
        explicit MoveCharacter(diji::GameObject* actor, const sf::Vector2f& direction);
        ~MoveCharacter() noexcept override = default;

        void Execute() override;

    private:
        const sf::Vector2f m_Direction;
        PlayerCharacter* m_Character = nullptr;
    };

    class CharacterJump final : public diji::GameActorCommands
    {
    public:
        explicit CharacterJump(diji::GameObject* actor, const bool isJumping);
        ~CharacterJump() noexcept override = default;

        void Execute() override;

    private:
        PlayerCharacter* m_Character = nullptr;
        bool m_IsJumping = false;
    };
}
