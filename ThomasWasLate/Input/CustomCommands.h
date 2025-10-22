#pragma once
#include "Engine/Input/GameActorCommand.h"

namespace thomasWasLate
{
    class tempTest;
}

namespace diji
{
    class Collider;
}

namespace thomasWasLate
{
    class PlayerCharacter;

    class MoveCharacter final : public diji::GameActorCommands
    {
    public:
        explicit MoveCharacter(diji::GameObject* actor, const sf::Vector2f& direction, bool isMoving);
        ~MoveCharacter() noexcept override = default;

        void Execute() override;

    private:
        const sf::Vector2f m_Direction;
        PlayerCharacter* m_Character = nullptr;
        bool m_IsMoving = false;
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

    class TempAddImpulse final : public diji::GameActorCommands
    {
    public:
        explicit TempAddImpulse(diji::GameObject* actor);
        ~TempAddImpulse() noexcept override = default;

        void Execute() override;

    private:
        diji::Collider* m_Collider = nullptr;
        float m_RandomForce = 2000.f;
    };

    class Sprint final : public diji::GameActorCommands
    {
    public:
        explicit Sprint(diji::GameObject* actor, bool isSprinting);
        ~Sprint() noexcept override = default;

        void Execute() override;
        
    private:
        PlayerCharacter* m_Character = nullptr;
        bool m_IsSprinting = false;
    };

    class Attack final : public diji::GameActorCommands
    {
    public:
        explicit Attack(diji::GameObject* actor);
        ~Attack() noexcept override = default;

        void Execute() override;
        
    private:
        PlayerCharacter* m_Character = nullptr;
    };

    class StartGame final : public diji::GameActorCommands
    {
    public:
        explicit StartGame(diji::GameObject* actor) : GameActorCommands{ actor } {};
        ~StartGame() noexcept override = default;

        void Execute() override;
    };
}
